# sdlgui — A Lightweight C++14 GUI Library Built on SDL3

[中文版](#中文版)

---

## English

### Overview

**sdlgui** is a minimal, header-only GUI library for C++14 built directly on top of SDL3. It provides a retained-mode widget system with a simple, chainable API. The entire library lives in a single `sdlgui.hpp` file, inspired by the stb-style single-header library pattern.

**Key features:**

- Single header: `sdlgui.hpp` — drop in and use
- Retained mode widget tree with automatic clipping
- Built-in Unifont bitmap font via `fontData.hpp` (supports Unicode BMP)
- Runtime font switching via `setFont()` / `Font` metrics, with cache and layout invalidation
- Opt-in content sizing (`setAutoHeight`/`setAutoWidth`) so text controls follow font metric changes
- `addChild<T>()` template — no manual `make_unique` + `move`
- Chainable position/size API: `pos()->sz()`
- Opt-in layout: edge anchors, HBox/VBox linear layout, draggable SplitView
- Built-in dark/light themes plus per-widget color APIs
- `RichTextView` for styled runs, code blocks, quotes and links
- Custom title bar with minimize, maximize, close buttons
- TextBox / TextArea / ComboBox editing with caret and clipboard support; TextBox also supports placeholder, password masking, and max length; TextBox/TextArea editing keys are rebindable; IME candidate window follows the caret
- Render-on-demand: idle windows skip Clear / Draw / Present
- Windowed applications with system or custom title bars via SDL3

### Requirements

- C++14 compiler
- SDL3 **3.2.0 or newer** (uses `SDL_RenderClipEnabled`, `SDL_GetTextureSize`)
- (Optional) `pkg-config` for easy build configuration

### Quick Start

```cpp
#define SDLGUI_IMPLEMENTATION
#define SDLGUI_USE_BUILTIN_FONT
#include "sdlgui.hpp"

int main()
{
    sdlgui::Application app;
    auto& win = app.addWindow("Hello", 800, 600);

    auto* btn = win.root().addChild<sdlgui::Button>();
    btn->pos(20, 80)->sz(120, 30);
    btn->setText("Click me");
    btn->onClick = [](sdlgui::MouseEvent&)
    {
        std::puts("Clicked!");
    };

    app.run();
    return 0;
}
```

Compile with:

```bash
g++ -std=c++14 $(pkg-config --cflags sdl3) -o demo demo.cc $(pkg-config --libs sdl3)
```

### File Structure

```
sdlgui/
├── sdlgui.hpp          # Main library (single header, stb-style)
├── fontData.hpp        # Bundled bitmap font data (8x16 + full BMP 16x16)
├── examples/
│   ├── demo.cc         # Widget showcase
│   └── counter.cc      # Minimal counter example
├── LICENSE             # BSD 2-Clause
├── Makefile            # Build configuration
└── readme.md           # Documentation (English + Chinese)
```

### Architecture

#### Widget Tree

sdlgui uses a retained-mode architecture. All UI elements inherit from `Widget`, which manages a tree of child widgets:

```
Application
  └── Window (owns SDL_Window + SDL_Renderer)
       └── root Widget
            ├── Label
            ├── Button
            ├── TextBox
            ├── CheckBox
            ├── ScrollView
            │    ├── Canvas
            │    └── Label
            └── ...
```

- Each window has one root widget.
- Parents own their children via `std::unique_ptr`.
- Children reference their parent via a raw pointer.
- Coordinates are **relative to parent**; absolute position is computed via `absoluteX()` / `absoluteY()`.
- Z-order is determined by insertion order: later children render on top.

#### Event Flow

```
SDL_PollEvent()
  → Application::run() interprets event type
    → handleMouseEvent() / handleKeyEvent()
      → Window::handleTitleBarEvent()    (custom title bar buttons)
      → hitTest()                        (recursive, topmost child first)
      → target->onMouseDown / onClick / onKeyDown etc.  (std::function callbacks)
```

Events are dispatched to exactly one widget (the topmost hit). No event bubbling. Each widget stores its callbacks as `std::function<void(...)>` members:

```cpp
button->onClick = [](MouseEvent& e) { /* ... */ };
```

#### Coordinate System

- `x()`, `y()`, `width()`, `height()` — relative to parent widget
- `absoluteX()`, `absoluteY()` — computed recursively from parent chain
- `absoluteRect()` — convenience for absolute position + size
- Hit testing and rendering use absolute coordinates internally

#### Render on Demand

Widgets are redrawn only when needed. `markDirty()` propagates changes up to the root widget, and `Window::render()` skips the entire frame (including `SDL_RenderClear` and `SDL_RenderPresent`) when the tree is clean. Text cursors invalidate at their blink phase, and overlay state changes (menus, context menus, message boxes, title bar hover) invalidate the window explicitly.

`Canvas` widgets with an `onDraw` callback request continuous redraw by default, preserving animated canvases. Call `setContinuousRedraw(false)` for static canvas content. Custom animations can call `Widget::requestRedraw()` / `Window::requestRedraw()`. Define `SDLGUI_ALWAYS_REDRAW` to force the previous every-frame behavior.

### Widget Reference

#### Widget (base class)

All UI elements inherit from `Widget`. It provides:

- Position/size management
- Parent-child tree operations (`addChild<T>()`, `addChild(unique_ptr)`, `removeChild()`)
- Event callbacks (all `std::function`, C++11 compatible)
- Dirty flag and `requestRedraw()`
- `addChild<T>(args...)` template for type-safe child creation

**Event callbacks:**

| Callback | Signature | Trigger |
|----------|-----------|---------|
| `onMouseDown` | `void(MouseEvent&)` | Mouse button pressed |
| `onMouseUp` | `void(MouseEvent&)` | Mouse button released |
| `onMouseMove` | `void(MouseEvent&)` | Mouse moved |
| `onClick` | `void(MouseEvent&)` | Press + release within bounds |
| `onMouseEnter` | `void(MouseEvent&)` | Mouse enters widget area |
| `onMouseLeave` | `void(MouseEvent&)` | Mouse leaves widget area |
| `onLongPress` | `void(MouseEvent&)` | Touch long press (no click is generated). If unset and `onContextMenu` is set, long press opens the context menu |
| `onKeyDown` | `void(KeyEvent&)` | Key pressed |
| `onKeyUp` | `void(KeyEvent&)` | Key released |
| `onFocus` | `void()` | Widget receives keyboard focus |
| `onBlur` | `void()` | Widget loses keyboard focus |
| `onContextMenu` | `void(MouseEvent&, ContextMenu&)` | Right-click — populate menu items |

**Chain methods:**

```cpp
Widget* pos(int x, int y);   // calls setPosition()
Widget* sz(int w, int h);    // calls setSize()
Widget* vis(bool v);         // calls setVisible()
Widget* ena(bool v);         // calls setEnabled()
Widget* anch(int flags);     // calls setAnchors()
Widget* stretch(float w);    // calls setStretch()
Widget* fillCross(bool on);  // calls setFillCross()
Widget* autoH(bool on);      // calls setAutoHeight()
Widget* autoW(bool on);      // calls setAutoWidth()
```

| Method | Returns | Description |
|--------|---------|-------------|
| `setEnabled(bool)` | `void` | Enable/disable widget interaction |
| `enabled()` | `bool` | Check if widget is enabled |
| `ena(bool)` | `Widget*` | Chainable setEnabled |
| `removeChild(Widget*)` | `void` | Remove and destroy a direct child |
| `detachChild(Widget*)` | `unique_ptr<Widget>` | Remove a direct child without destroying it |
| `reparent(Widget*)` | `void` | Move this widget under another parent (keeps position/size) |
| `setFocusable(bool)` | `void` | Include the widget in Tab traversal when enabled |
| `focusable()` | `bool` | Whether the widget participates in keyboard focus |
| `setAutoHeight(bool)` / `setAutoWidth(bool)` | `void` | Let the widget derive that dimension from its content |
| `preferredHeight(width)` / `preferredWidth(height)` | `int` | Content size hint (defaults to the current size) |
| `requestRedraw()` | `void` | Ask the owning window to draw another frame |

Auto-sizing is opt-in and preserves the fixed-rectangle meaning of `sz()`. `Label`, `Button`, `CheckBox` and `Box` implement `preferredHeight()`/`preferredWidth()`; auto-sized widgets are re-measured when the font changes, so text is not clipped after switching to a taller font.

Disabled widgets reject hit tests (unclickable) and render dimmed. Disabling a parent disables all children.

#### Layout

Widgets use absolute positioning by default. Layout is opt-in and can be mixed with manual positions.

**Anchors**

```cpp
auto* editor = root.addChild<sdlgui::Widget>();
editor->pos(160, 40)->sz(400, 300);
editor->setAnchors(sdlgui::AnchorLeft | sdlgui::AnchorTop |
                   sdlgui::AnchorRight | sdlgui::AnchorBottom);
```

`setAnchors(flags)` captures the current margins from the parent. A left+right pair stretches the width, top+bottom stretches the height, and a right-only/bottom-only anchor keeps the distance to that edge. You can also pass explicit margins:

```cpp
panel->setAnchors(sdlgui::AnchorLeft | sdlgui::AnchorTop, 8, 8, 0, 0);
```

| Method | Returns | Description |
|--------|---------|-------------|
| `setAnchors(flags)` | `void` | Anchor to edges, capturing current margins |
| `setAnchors(flags,l,t,r,b)` | `void` | Anchor with explicit margins |
| `setAnchorMargins(l,t,r,b)` | `void` | Change margins and re-layout |
| `anchors()` | `int` | Current anchor flags |
| `relayout()` | `void` | Force layout of this widget's children |
| `anch(flags)` | `Widget*` | Chainable `setAnchors` |

**HBox / VBox**

```cpp
auto* row = root.addChild<sdlgui::HBox>();
row->pos(0, 0)->sz(600, 36);
row->setPadding(4);
row->setSpacing(6);

auto* open = row->addChild<sdlgui::Button>();
open->sz(80, 28);
open->txt("Open");

auto* path = row->addChild<sdlgui::TextBox>();
path->stretch(1.0f);       // take the remaining width
path->fillCross(true);     // fill the row height
```

- `HBox` lays out left-to-right, `VBox` top-to-bottom.
- Fixed-size children keep their current main-axis size.
- `setStretch(weight)` distributes the remaining main-axis space.
- `setFillCross(true)` fills the perpendicular axis; otherwise the child keeps its own size.
- Invisible children are skipped.
- HBox/VBox manage their direct children, so anchors on those children are ignored.

**SplitView**

```cpp
auto* split = root.addChild<sdlgui::SplitView>();
split->setAnchors(sdlgui::AnchorLeft | sdlgui::AnchorTop |
                  sdlgui::AnchorRight | sdlgui::AnchorBottom);
split->setOrientation(sdlgui::SplitView::Orientation::SideBySide);
split->setSplit(0.3f);
split->setDividerWidth(6);

auto* sidebar = split->addChild<sdlgui::VBox>(); // first visible child
auto* editor  = split->addChild<sdlgui::VBox>(); // second visible child
```

`SideBySide` splits left/right; `Stacked` splits top/bottom. Drag the divider to resize; `setMinPaneSize()` limits both panes. `setDividerHitWidth(px)` separates the visual divider width from the drag hit area (`0` = `max(dividerWidth, 6)`).

**Drawer**

A full-size overlay panel with a scrim. Add it as the last child of the root so it covers the other widgets:

```cpp
auto* drawer = root.addChild<sdlgui::Drawer>();
drawer->setAnchors(sdlgui::AnchorLeft | sdlgui::AnchorTop |
                   sdlgui::AnchorRight | sdlgui::AnchorBottom);
drawer->setPanelWidth(260);
drawer->setSide(sdlgui::Drawer::Side::Left);

auto* list = drawer->panel()->addChild<sdlgui::ListBox>();
list->pos(10, 10)->sz(240, 400);

drawer->open();    // or close() / toggle()
drawer->onOpenChanged = [](bool open) { /* ... */ };
```

Clicks on the scrim close the drawer without activating the widget underneath. The panel content is a normal widget tree (`drawer->panel()`). `drawer->setCloseButtonVisible(true)` adds a built-in close button at the panel's top-right. `setScrimColor()` and `setPanelColor()` alpha values are honored (the scrim is blended over the content behind the drawer).

#### Label

Displays static text.

```cpp
auto* label = root.addChild<sdlgui::Label>();
label->pos(20, 40)->sz(200, 20);
label->setText("Hello, world!");
label->setTextColor(sdlgui::Color::Green);
```

| Method | Returns | Description |
|--------|---------|-------------|
| `setText(string)` | `void` | Set text content |
| `setTextColor(Color)` | `void` | Set text color |
| `setTextAlign(RichTextAlign)` | `void` | Left / Center / Right per line |
| `txt(string)` | `Widget*` | Chainable setText |
| `clr(Color)` | `Widget*` | Chainable setTextColor |

#### Button

A clickable button with three visual states: Normal, Hover, Pressed.

```cpp
auto* btn = root.addChild<sdlgui::Button>();
btn->pos(20, 80)->sz(120, 30);
btn->setText("Click");
btn->setBgColor({60,60,60}, {80,80,80}, {40,40,40});  // normal, hover, pressed
btn->onClick = [](sdlgui::MouseEvent&) { /* ... */ };
```

| Method | Returns | Description |
|--------|---------|-------------|
| `setText(string)` | `void` | Set button text |
| `setTextColor(Color)` | `void` | Set text color |
| `setBgColor(Color,Color,Color)` | `void` | Set normal, hover, pressed colors |
| `txt(string)` | `Widget*` | Chainable setText |
| `clr(Color)` | `Widget*` | Chainable setTextColor |
| `bg(Color,Color,Color)` | `Widget*` | Chainable setBgColor |
| `onClick` | callback | Fired on mouse release within bounds |

The button automatically manages its state via internal `onMouseEnter`, `onMouseLeave`, `onMouseDown`, `onMouseUp` handlers.

#### CheckBox

A toggle checkbox with label.

```cpp
auto* cb = root.addChild<sdlgui::CheckBox>();
cb->pos(20, 130)->sz(160, 24);
cb->setText("Enable option");
```

CheckBox toggles itself on click (and with Space/Enter when focused); there is no need to override `onClick`.

| Method | Returns | Description |
|--------|---------|-------------|
| `setText(string)` | `void` | Set label text |
| `setChecked(bool)` | `void` | Set checked state |
| `setTextColor(Color)` | `void` | Set label text color |
| `setCheckColor(Color)` | `void` | Set check mark color |
| `txt(string)` | `Widget*` | Chainable setText |
| `clr(Color)` | `Widget*` | Chainable setTextColor |
| `checkClr(Color)` | `Widget*` | Chainable setCheckColor |

#### TextBox

A single-line text input field with cursor, text selection, editing, and auto-scroll. Click to place the caret; drag (or Shift+click) to select text.

```cpp
auto* tb = root.addChild<sdlgui::TextBox>();
tb->pos(20, 180)->sz(200, 28);
tb->setPlaceholder("type here");
tb->setMaxLength(32);

auto* pw = root.addChild<sdlgui::TextBox>();
pw->pos(20, 216)->sz(200, 28);
pw->setPassword(true);
pw->setPlaceholder("password");
```

| Method | Returns | Description |
|--------|---------|-------------|
| `setText(string)` | `void` | Set text content and move cursor to end |
| `setTextColor(Color)` | `void` | Set text color |
| `setBackgroundColor(Color)` | `void` | Set background color |
| `setCursorColor(Color)` | `void` | Set cursor color |
| `setSelectionColor(Color)` | `void` | Set selection highlight color |
| `setPlaceholder(string)` | `void` | Placeholder shown while the text is empty |
| `setPlaceholderColor(Color)` | `void` | Set placeholder color |
| `setContentInsets(l,t,r,b)` | `void` | Text/caret/selection inner insets |
| `setBorderVisible(bool)` / `setBackgroundVisible(bool)` | `void` | Toggle border/background painting |
| `setMaxLength(int)` | `void` | Limit to N Unicode codepoints; `0` = unlimited |
| `setPassword(bool)` | `void` | Mask the displayed text |
| `setPasswordChar(char)` | `void` | Set mask character (default `*`) |
| `insertText(string)` | `void` | Insert text, replacing selection and respecting max length |
| `scrollByPixels(int)` | `bool` | Scroll the visible text horizontally; returns whether it moved |
| `adjustScroll()` | `void` | Ensure cursor is visible (auto called) |
| `cursorPosition()` | `int` | Get caret byte position |
| `hasSelection()` | `bool` | Whether text is selected |
| `selectedText()` | `string` | Get selected text |
| `selectAll()` / `clearSelection()` | `void` | Select all / clear selection |
| `txt(string)` | `Widget*` | Chainable setText |
| `clr(Color)` | `Widget*` | Chainable setTextColor |
| `bgClr(Color)` | `Widget*` | Chainable setBackgroundColor |
| `ph(string)` / `maxLen(int)` / `pwd(bool)` | `Widget*` | Chainable placeholder / max length / password |

**Keyboard controls:**

| Key | Action |
|-----|--------|
| Any printable key | Replace selection (or insert at cursor) |
| Backspace / Delete | Delete selection, or one character |
| Left/Right arrows | Move cursor and clear selection |
| Shift+Left/Right/Home/End | Extend selection |
| Home / End | Move cursor to beginning / end |
| Ctrl+A | Select all |
| Ctrl+C | Copy selection (no-op if nothing selected) |
| Ctrl+X | Cut selection (no-op if nothing selected) |
| Ctrl+V | Paste at cursor, replacing selection |
| Esc | Clear selection |

All editing shortcuts are per-widget rebindable; see **Rebindable Keys** below.

The TextBox automatically scrolls horizontally when text exceeds the visible area, keeping the cursor visible. A horizontal wheel/trackpad gesture or Shift+wheel scrolls it manually. Password display is masked per Unicode character while `text()` and `selectedText()` still return the real text.

#### ScrollView

A scrollable viewport for content larger than the visible area.

```cpp
auto* sv = root.addChild<sdlgui::ScrollView>();
sv->pos(20, 240)->sz(300, 200);
sv->setContentSize(600, 400);
// Add children to sv - they will be clipped and scrollable
```

| Method | Returns | Description |
|--------|---------|-------------|
| `setContentSize(w, h)` | `void` | Set total virtual content size |
| `setScrollOffset(x, y)` | `void` | Set scroll position (automatically clamped) |
| `scrollbarV(bool)` | `void` | Show/hide vertical scrollbar (default: hidden) |
| `scrollbarH(bool)` | `void` | Show/hide horizontal scrollbar (default: hidden) |
| `sbV(bool)` | `ScrollView*` | Chainable scrollbarV |
| `sbH(bool)` | `ScrollView*` | Chainable scrollbarH |

ScrollView clips its children to its own bounds using `SDL_SetRenderClipRect`. Content is temporarily offset during rendering. Hit-testing properly accounts for scroll offset.

Scrollbars are **hidden by default** — you must call `scrollbarV(true)` or `scrollbarH(true)` to show them (they only appear when content exceeds the view area). Mouse wheel scrolling works regardless of scrollbar visibility.

#### Canvas

A widget that exposes the raw SDL_Renderer for custom drawing via a callback.

```cpp
auto* cv = root.addChild<sdlgui::Canvas>();
cv->pos(20, 240)->sz(300, 200);
cv->onDraw = [](SDL_Renderer* r, const sdlgui::Rect& area)
{
    SDL_SetRenderDrawColor(r, 255, 0, 0, 255);
    SDL_RenderLine(r, area.x, area.y, area.x + area.w, area.y + area.h);
};
```

**`onDraw` callback signature:** `void(SDL_Renderer*, const Rect& clipArea)`

The `clipArea` parameter contains the absolute position and size of the canvas, which is the same as the clip rectangle set before calling `onDraw`.

**Redraw policy:** a Canvas with `onDraw` set redraws every frame by default. For static content call `setContinuousRedraw(false)`; then call `requestRedraw()` (or `markDirty()`) when its content actually changes. This is what `examples/demo.cc` does for its static grid/circle.

#### Window

Represents an OS-level window with its own SDL_Window and SDL_Renderer.

```cpp
auto& win = app.addWindow("My App", 800, 600);
```

**Title bar modes:**

| Mode | Description |
|------|-------------|
| `TitleBarMode::System` | Native OS title bar (default) |
| `TitleBarMode::Custom` | Custom title bar drawn by sdlgui |
| `TitleBarMode::Hidden` | No title bar, full client area |

```cpp
win.setTitleBarMode(sdlgui::Window::TitleBarMode::Custom);
win.setBackgroundColor(sdlgui::Color(30, 30, 40));
```

**Custom title bar buttons:**

| Button | Color (default) | Color (hover) | Action |
|--------|-----------------|---------------|--------|
| Minimize `_` | Transparent | Bright gray | `SDL_MinimizeWindow` |
| Maximize `□` | Transparent | Bright gray | `SDL_MaximizeWindow` / `SDL_RestoreWindow` |
| Close `X` | Transparent | Red bg | `SDL_PushEvent(SDL_EVENT_QUIT)` |

| Method | Returns | Description |
|--------|---------|-------------|
| `setTitle(const char*)` | `void` | Set window title |
| `setTitleBarMode(TitleBarMode)` | `void` | Switch title bar mode |
| `setTitleBarHeight(int)` | `void` | Set custom title bar height |
| `setTitleIconTexture(SDL_Texture*)` | `void` | Set icon from texture (takes ownership) |
| `setTitleIconSurface(SDL_Surface*)` | `bool` | Set icon from surface (takes ownership) |
| `setTitleIconMemory(data, size)` | `bool` | Set icon from embedded PNG/BMP bytes |
| `setTitleIconPixels(w, h, rgba)` | `bool` | Set icon from RGBA pixel data |
| `clearTitleIcon()` | `void` | Remove the title bar icon |
| `setTitleIconSize(int)` | `void` | Icon box size; `0` = automatic |
| `setTitleIconPadding(left, gap)` | `void` | Left padding and title gap |
| `setWindowIconMemory(data, size)` | `bool` | Set the OS/taskbar icon from PNG/BMP bytes |
| `setBackgroundColor(Color)` | `void` | Set window background color |
| `setResizable(bool)` | `void` | Toggle window resizability at runtime |
| `requestRedraw()` | `void` | Force another frame to be rendered |
| `needsRedraw()` | `bool` | Whether the next `render()` would actually draw |
| `valid()` | `bool` | Whether SDL window/renderer creation succeeded |
| `root()` | `Widget&` | Get root widget for adding children |
| `handle()` | `SDL_Window*` | Get native SDL window handle |
| `renderer()` | `SDL_Renderer*` | Get native SDL renderer |

Borderless `Custom` / `Hidden` windows use native edge hit-testing: hovering the outer 6px (edges and corners) shows the OS resize cursor and dragging resizes the window. Call `setResizable(false)` to disable it. Switching title bar modes at runtime installs or removes the hit test automatically.

The custom title bar can show an icon on the left. For single-file distribution, embed a PNG in the executable instead of shipping an `.ico`:

```bash
xxd -i app_icon.png > app_icon.h
```

```cpp
win.setTitleBarMode(sdlgui::Window::TitleBarMode::Custom);
win.setTitleIconMemory(app_icon_png, app_icon_png_len);
win.setWindowIconMemory(app_icon_png, app_icon_png_len); // taskbar/dock

win.onTitleIconClick = [] {
    // optional: open an app menu
};
```

`setTitleIconMemory()` accepts PNG or BMP without requiring SDL3_image. Texture/surface setters take ownership; memory data is only read during the call.

#### Application

Manages windows and the main event loop.

```cpp
sdlgui::Application app;
auto& win = app.addWindow("Title", 800, 600);
app.run();  // Blocks until quit
```

| Method | Returns | Description |
|--------|---------|-------------|
| `addWindow(title, w, h)` | `Window&` | Create a new window |
| `run()` | `void` | Enter the main event loop (blocking) |
| `quit()` | `void` | Request exit from event loop |
| `isRunning()` | `bool` | Check if event loop is active |
| `processEvents()` | `void` | Pump one frame of events (non-blocking) |
| `setFocus(Widget*)` | `void` | Set keyboard focus to a widget |
| `focused()` | `Widget*` | Get the currently focused widget |
| `valid()` | `bool` | Whether `SDL_Init` succeeded |
| `setFPS(fps)` | `void` | Set target FPS (0 = unlimited, default 60) |
| `setUiScale(scale)` | `void` | Logical UI zoom; notifies the current font via `Font::setRasterScale` |
| `fps()` | `int` | Get current target FPS |
| `setTouchMode(bool)` | `void` | Force mobile/touch behavior |
| `setTouchTargetPadding(px)` | `void` | Extra hit-test padding in touch mode |
| `setKeyboardInset(px)` | `void` | Reserve bottom space for an on-screen keyboard |

**Callbacks:**

| Callback | Signature | Description |
|----------|-----------|-------------|
| `onUserEvent` | `void(const SDL_Event&)` | Hook for unhandled SDL events (dispatched at end of processEvents) |

The event loop polls events at the configured FPS (default 60), but idle windows are not redrawn. `setFPS(30)` reduces the polling/animation rate; `setFPS(0)` removes the frame delay. `processEvents()` is public and can be called manually for custom event loops (used internally by blocking dialogs).

#### Keyboard Focus

- `Tab` / `Shift+Tab` focus traversal is **opt-in**; enable it with `app.setTabTraversal(true)`. It cycles focusable widgets (TextBox, TextArea, ComboBox, Button, CheckBox, ListBox, Slider; custom widgets via `setFocusable(true)`)
- A thin blue focus ring is drawn only for keyboard focus; mouse clicks do not draw it (TextBox has no focus border and relies on its caret)
- Button: `Enter` / `Space` activates; CheckBox: `Enter` / `Space` toggles
- ListBox: `Up` / `Down` changes the selected item
- Slider: `Left` / `Right` steps the value; `Home` / `End` jumps to min/max
- `Esc` closes the context menu / menu bar dropdown and dismisses message boxes; `Enter` accepts the default message box button

#### ListBox

A single-selection list of text items.

```cpp
auto* lb = root.addChild<sdlgui::ListBox>();
lb->pos(20, 400)->sz(200, 150);
lb->addItem("Item A");
lb->addItem("Item B");
lb->setSelectedIndex(0);
lb->onItemSelected = [](int index) { /* ... */ };
```

| Method | Returns | Description |
|--------|---------|-------------|
| `addItem(string)` | `void` | Add an item |
| `removeItem(index)` | `void` | Remove item at index |
| `clear()` | `void` | Remove all items |
| `selectedIndex()` | `int` | Get selected index (-1 if none) |
| `selectedItem()` | `string&` | Get selected item text |
| `onItemSelected` | callback | Fired when an item is clicked |
| `onItemActivated` | callback | Fired on double-click or Enter (`void(int)`) |

#### Slider

A horizontal slider for floating-point values.

```cpp
auto* sl = root.addChild<sdlgui::Slider>();
sl->pos(20, 560)->sz(200, 24);
sl->setRange(0.f, 100.f);
sl->setValue(50.f);
sl->onValueChanged = [](float val) { /* ... */ };
```

| Method | Returns | Description |
|--------|---------|-------------|
| `setRange(min, max)` | `void` | Set value range (float) |
| `setValue(val)` | `void` | Set current value (float) |
| `value()` | `float` | Get current value |
| `onValueChanged` | callback | Fired on value change during drag `void(float)` |
| `onDragChanged` | callback | Fired when drag starts (true) or ends (false) |

#### DropDown

A dropdown selection list.

```cpp
auto* dd = root.addChild<sdlgui::DropDown>();
dd->pos(20, 600)->sz(160, 24);
dd->addItem("Option A");
dd->addItem("Option B");
dd->setSelectedIndex(0);
dd->onItemSelected = [](int index) { /* ... */ };
```

| Method | Returns | Description |
|--------|---------|-------------|
| `addItem(string)` | `void` | Add an item |
| `clear()` | `void` | Remove all items |
| `selectedIndex()` | `int` | Get selected index |
| `setSelectedIndex(int)` | `void` | Set selected index |
| `onItemSelected` | callback | Fired on selection |

The expanded list is rendered as an overlay above all regular widgets, including siblings added later. Clicks inside the list take priority; clicking outside only dismisses the list without activating the widget underneath. If there is not enough space below, the list flips above the control.

#### ComboBox

An editable text field with a dropdown list.

```cpp
auto* cb = root.addChild<sdlgui::ComboBox>();
cb->pos(20, 640)->sz(200, 24);
cb->addItem("Apple");
cb->addItem("Banana");
cb->setSelectedIndex(0);
cb->onItemSelected = [](int index) { /* ... */ };
cb->onTextChanged  = [](const std::string& text) { /* ... */ };
```

The text area is editable: typing, Backspace/Delete, Left/Right/Home/End and Ctrl+V work while focused. Clicking inside the text area places the caret at the clicked column. Opening the dropdown and clicking outside closes it automatically.

| Method | Returns | Description |
|--------|---------|-------------|
| `addItem(string)` | `void` | Add an item to the dropdown |
| `clear()` | `void` | Remove all items and text |
| `selectedIndex()` | `int` | Get selected index |
| `setSelectedIndex(int)` | `void` | Select an item and copy it into the text field |
| `setText(string)` / `text()` | `void` / `string` | Set/get the current text |
| `onItemSelected` | callback | `void(int)` when an item is chosen |
| `onTextChanged` | callback | `void(const string&)` when the text is edited |
| `isPopupOpen()` / `closePopup()` | `bool` / `void` | Query/close the dropdown |

#### ContextMenu

A right-click context menu with multiple items.

```cpp
widget->onContextMenu = [](sdlgui::MouseEvent&, sdlgui::ContextMenu& menu)
{
    menu.addItem("Copy",   []() { /* ... */ });
    menu.addItem("Paste", []() { /* ... */ });
    menu.addSeparator();
    menu.addItem("Delete", []() { /* ... */ });
};
```

| Method | Returns | Description |
|--------|---------|-------------|
| `addItem(text, callback)` | `void` | Add a clickable menu item |
| `addSeparator()` | `void` | Add a horizontal separator line |
| `clear()` | `void` | Remove all items |
| `show(x, y)` | `void` | Show the menu at screen position |
| `dismiss()` | `void` | Hide the menu |
| `onDismiss` | callback | Fired when menu is dismissed |

The menu automatically dismisses when clicking outside of it. Each widget can have its own `onContextMenu` callback with different items. Row height follows the current font (`itemHeight()`, at least 22px), so larger fonts stay readable; an open menu is re-fitted when the font changes.

#### MenuBar

A horizontal menu bar with clickable menu titles and dropdown menus. Supports sub-items, separators, and keyboard shortcut hints.

```cpp
auto* mb = root.addChild<sdlgui::MenuBar>();
mb->pos(0, 0)->sz(800, 24);

int file = mb->addMenu("File");
mb->addItem(file, "New",    []() { /* ... */ }, "Ctrl+N");
mb->addSeparator(file);
mb->addItem(file, "Open",   []() { /* ... */ }, "Ctrl+O");
mb->addItem(file, "Save",   []() { /* ... */ }, "Ctrl+S");

int edit = mb->addMenu("Edit");
mb->addItem(edit, "Undo",   []() { /* ... */ }, "Ctrl+Z");
mb->addItem(edit, "Redo",   []() { /* ... */ }, "Ctrl+Shift+Z");
```

| Method | Returns | Description |
|--------|---------|-------------|
| `addMenu(title)` | `int` | Add a menu title, returns menu index |
| `addItem(menuIdx, text, onClick, shortcut)` | `int` | Add an item with optional shortcut hint |
| `addSeparator(menuIdx)` | `int` | Add a separator line in a menu |
| `clearMenu(menuIdx)` | `void` | Remove all items from a menu |
| `clearAll()` | `void` | Remove all menus |
| `closeAll()` | `void` | Close any open dropdown |
| `onItemSelected` | callback | Fired when any item is clicked `(menuIdx, itemIdx)` |

**Behavior:**
- Click a menu title to open its dropdown; click another title to switch.
- Hover over a title when a menu is open auto-switches to that menu.
- Click outside the dropdown (or click an item) to close.
- Keyboard shortcut hints are right-aligned in the dropdown.
- Default height is 24px.

#### MessageBox

A modal dialog box with icons, message text, and buttons. Rendered as an overlay on top of all window content.

**Callback (non-blocking):**

```cpp
win.showMessageBox("Error", "File not found!",
    sdlgui::MessageBoxType::Error,
    sdlgui::MessageBoxButtons::OKCancel,
    [](sdlgui::MessageBoxResult r) {
        if (r == sdlgui::MessageBoxResult::OK) { /* ... */ }
    });
```

**Blocking version (runs its own event loop until dismissed):**

```cpp
sdlgui::MessageBoxResult r = win.showMessageBoxBlocking(
    "Confirm", "Delete file?",
    sdlgui::MessageBoxType::Question,
    sdlgui::MessageBoxButtons::YesNo);
```

**Message box types:**

| Type | Icon | Description |
|------|------|-------------|
| `Info` | (i) | Informational message |
| `Warning` | (!) | Warning message |
| `Error` | (X) | Error message |
| `Question` | (?) | Confirmation prompt |

**Button combinations:**

| Buttons | Available results |
|---------|------------------|
| `OK` | OK |
| `OKCancel` | OK, Cancel |
| `YesNo` | Yes, No |
| `YesNoCancel` | Yes, No, Cancel |
| `RetryCancel` | Retry, Cancel |

| Method | Returns | Description |
|--------|---------|-------------|
| `showMessageBox(title, msg, type, buttons, callback)` | `void` | Show non-blocking message box |
| `showMessageBoxBlocking(title, msg, type, buttons)` | `MessageBoxResult` | Show blocking message box |

Clicking the semi-transparent overlay (outside the dialog) returns `MessageBoxResult::None`. When a message box is active, all title bar interactions are blocked.

#### TextArea

Multi-line editable text view (inherits ScrollView). Word wrap is **on by default**; a vertical scrollbar is shown as needed, and a horizontal scrollbar appears when wrapping is disabled. Supports selection, drag-select, Shift+click, clipboard and read-only mode.

```cpp
auto* ta = root.addChild<sdlgui::TextArea>();
ta->pos(300, 400)->sz(300, 200);
ta->setText("Line 1\nLine 2\nLine 3");
```

| Method | Returns | Description |
|--------|---------|-------------|
| `setText(string)` | `void` | Set text content and reset the caret |
| `setTextColor(Color)` | `void` | Set text color |
| `setSelectionColor(Color)` | `void` | Set selection highlight color |
| `setLineSpacing(int)` | `void` | Extra pixels between lines (0 = font default) |
| `setLetterSpacing(int)` | `void` | Extra pixels between characters (0 = font default) |
| `setWordWrap(bool)` | `void` | Enable/disable automatic wrapping (default `true`) |
| `setContentInsets(l,t,r,b)` | `void` | Text/caret/selection inner insets |
| `setBorderVisible(bool)` / `setBackgroundVisible(bool)` | `void` | Toggle border/background painting |
| `wordWrap()` | `bool` | Whether wrapping is enabled |
| `setShowLineNumbers(bool)` | `void` | Show a line-number gutter |
| `setAutoIndent(bool)` | `void` | Enter copies the current line's indentation; adds a level after `{`, `[` or `(` |
| `setSmartBackspace(bool)` | `void` | Backspace inside leading whitespace removes a whole soft-tab stop |
| `setIndentWithSpaces(bool)` | `void` | Auto-indent uses spaces (`true`) or tabs (`false`) |
| `setTabWidth(int)` | `void` | Tab width used by auto-indent and smart backspace (1-16) |
| `setReadOnly(bool)` | `void` | Toggle editing (selection/copy still work) |
| `text()` | `const string&` | Get current text |
| `hasSelection()` | `bool` | Whether a range is selected |
| `selectedText()` | `string` | Get selected text (includes newlines) |
| `selectAll()` / `clearSelection()` | `void` | Select all / clear selection |
| `setCursor(line, column)` | `void` | Set caret position (byte column) |
| `cursorLine()` / `cursorColumn()` | `int` | Get caret line / byte column |
| `insertText(string)` | `void` | Insert text, replacing the selection |
| `insertNewline()` | `void` | Insert a newline using the auto-indent rules |
| `backspace()` / `deleteForward()` | `void` | Delete selection, or one character |
| `moveCursorLeft/Right/Up/Down/Home/End(bool select = false)` | `void` | Move the caret; `true` extends the selection |

**Keyboard controls:** Shift+arrows/Home/End extends selection; Ctrl+A selects all; Ctrl+C/Ctrl+X copy/cut; Ctrl+V pastes and replaces the selection; Enter inserts a newline (with auto-indent when enabled). Mouse drag or Shift+click also selects.

Word wrap breaks at spaces/tabs and common punctuation when possible; CJK text falls back to character wrapping. `setWordWrap(false)` restores horizontal scrolling. Cursor positions still use logical line/byte-column coordinates.

Line and letter spacing are TextArea-only adjustments:

```cpp
ta->setLineSpacing(4);   // +4px per line
ta->setLetterSpacing(1); // +1px between characters
```

They affect rendering, caret/click mapping, selection, horizontal scrolling and IME caret positioning.

Editable when focused; `readOnly(true)` blocks text changes but still allows selection and copying. The caret blinks while the TextArea has keyboard focus.

#### Rebindable Keys (TextBox / TextArea)

Both text widgets own a `TextKeyMap`; the defaults are the shortcuts listed above.

```cpp
auto* tb = root.addChild<sdlgui::TextBox>();

// Replace the binding for one action:
tb->setKeyBinding(sdlgui::TextEditAction::Copy, SDLK_C,
                  SDL_KMOD_CTRL | SDL_KMOD_SHIFT);

// Add an extra chord for an action:
tb->keyMap().add(sdlgui::TextEditAction::Paste, SDLK_INSERT, SDL_KMOD_SHIFT);

// Disable one action, disable all shortcuts, or restore defaults:
tb->keyMap().remove(sdlgui::TextEditAction::Cut);
tb->keyMap().clear();
tb->resetKeyBindings();
```

Available actions: `CursorLeft/Right/Up/Down/Home/End`, `SelectLeft/Right/Up/Down/Home/End`, `SelectAll`, `Copy`, `Cut`, `Paste`, `Backspace`, `DeleteForward`, `InsertNewline`, `ClearSelection`. TextBox ignores cursor-up/down and newline. Only Shift/Ctrl/Alt/GUI are compared; left/right variants (e.g. `LCTRL` / `RCTRL`) are folded into the generic modifier, and Caps Lock / Num Lock are ignored. `Tab` is not a text-edit action; when `app.setTabTraversal(true)` is enabled it is consumed by focus traversal.

#### RichTextView

A rich text renderer built from blocks and styled runs. It does **not** parse Markdown; feed it blocks/runs from your own parser.

```cpp
auto* rt = root.addChild<sdlgui::RichTextView>();
rt->pos(20, 60)->sz(500, 300);

sdlgui::RichTextRun normal;
normal.text = "Hello ";

sdlgui::RichTextRun bold;
bold.text = "world";
bold.style.color = sdlgui::Color::White;

sdlgui::RichTextStyle codeStyle;
codeStyle.background = sdlgui::Color(60, 60, 70);

sdlgui::RichTextRun code;
code.text = " inline ";
code.style = codeStyle;

sdlgui::RichTextRun link;
link.text = "docs";
link.href = "https://example.com";

rt->addHeading({ normal }, 1);
rt->addParagraph({ normal, bold, code, link });
rt->addListItem({ normal });
rt->addQuote({ normal });
rt->addCodeBlock("int main() {\n  return 0;\n}", codeStyle);

rt->onLinkClick = [](const std::string& href) {
    // open href
};
```

| Method | Returns | Description |
|--------|---------|-------------|
| `addBlock(block)` | `int` | Add a block and return its index |
| `addParagraph(runs)` / `addHeading(runs, level)` | `int` | Add paragraph / heading |
| `addCodeBlock(text, style)` | `int` | Add a non-wrapping code block |
| `addListItem(runs, bullet)` / `addQuote(runs)` | `int` | Add list item / quote |
| `addTable(table)` | `int` | Add a simple table |
| `setBlocks(blocks)` / `clear()` | `void` | Replace / clear content |
| `blocks()` | `const vector<RichTextBlock>&` | Read blocks |
| `setPadding` / `setLineSpacing` | `void` | Layout spacing |
| `setDefaultTextColor` / `setLinkColor` | `void` | Default text / link color |
| `setCodeBackground` / `setCodeBorder` / `setQuoteBarColor` | `void` | Block styling |
| `setTableColors(row, header, border)` | `void` | Table cell colors |
| `setSeparatorColor(Color)` | `void` | Default separator color |
| `setActionColors(bg, text)` | `void` | Default action-row colors |
| `setSelectable(bool)` / `selectable()` | `void` / `bool` | Enable/disable selection (default `true`) |
| `setSelectionColor(Color)` | `void` | Selection highlight color |
| `selectAll()` / `clearSelection()` | `void` | Select all / clear selection |
| `selectedText()` | `string` | Selected text, including block newlines |
| `relayout()` | `void` | Force re-layout |
| `onLinkClick` | callback | Fired with the run's `href` |
| `onEmptyState` | callback | Drawn when `blocks()` is empty |

`RichTextStyle.font` is a `std::shared_ptr<Font>`; `nullptr` uses `currentFont()`. A color with `alpha == 0` means "use the view default". `RichTextStyle.background` draws an inline background, and `underline`/`href` draw an underline.

When the view has focus: `Ctrl+C` / `Cmd+C` copies the selection, `Ctrl+A` / `Cmd+A` selects all, and `Esc` clears it. `setSelectable(false)` disables these shortcuts as well as mouse selection.

Blocks can also be placed horizontally, which is useful for message bubbles:

```cpp
sdlgui::RichTextBlock msg;
msg.runs = { yourRun };
msg.align = sdlgui::RichTextAlign::Right;
msg.maxWidth = 320;       // total block width limit, including paddingX
msg.paddingX = 12;        // inner horizontal padding
msg.background = sdlgui::Color(42, 48, 60);
rt->addBlock(msg);
```

- `align` is `Left` / `Center` / `Right`.
- `maxWidth == 0`: left-aligned blocks fill the available width; centered/right-aligned blocks shrink to their content.
- `maxWidth > 0`: the block shrinks to its content and is capped at `maxWidth`.
- `paddingX` adds space inside the block background and reduces the wrapping width.
- Text wrapping uses the reduced content width, so resizing the view re-flows the block correctly.
- Tables currently keep their full-width layout.

**Separator**

```cpp
sdlgui::RichTextBlock line;
line.type = sdlgui::RichBlockType::Separator;
line.separatorThickness = 1;
line.separatorColor = sdlgui::Color(80, 80, 86);
line.separatorMarginTop = 6;
line.separatorMarginBottom = 6;
rt->addBlock(line);
```

A separator spans the available block width and is laid out in the normal vertical flow. It has no text and does not participate in selection.

**Card**

```cpp
sdlgui::RichTextBlock card;
card.isCard = true;
card.background = sdlgui::Color(30, 32, 40);
card.borderWidth = 1;
card.borderColor = sdlgui::Color(70, 72, 82);
card.paddingX = 12;
card.paddingY = 10;

card.children.push_back(titleBlock);
card.children.push_back(codeBlock);
card.children.push_back(separatorBlock);
card.children.push_back(resultBlock);
rt->addBlock(card);
```

Children are laid out vertically inside one background/border. Nested cards are supported. The card itself contributes no text; its children participate in selection normally.

**Actions**

```cpp
sdlgui::RichTextAction allow;
allow.text = "Allow";
allow.textAlign = sdlgui::RichTextAlign::Left;
allow.paddingX = 12;
allow.onClick = [] { /* ... */ };

sdlgui::RichTextAction deny;
deny.text = "Deny";
deny.enabled = false;

sdlgui::RichTextBlock actions;
actions.type = sdlgui::RichBlockType::Actions;
actions.actions = { allow, deny };
actions.actionsVertical = true;
actions.actionHeight = 32;
actions.actionGap = 8;
rt->addBlock(actions);
```

Actions are drawn in the content flow, scroll with the view, and are hit-tested by `RichTextView`. They are not part of the selectable text, so copy/selection does not include button labels. They are action rows rather than focusable child widgets; for keyboard-driven flows, use a normal `Button` outside `RichTextView`.

Tables use `RichTable`, `RichTableRow` and `RichTableCell`:

```cpp
sdlgui::RichTable table;
table.headerRow = true;
table.cellPadding = 6;
table.columnWeights = { 1.0f, 2.0f }; // relative widths

sdlgui::RichTableRow header;
header.cells = { { { "Name", {}, "" } }, { { "Description", {}, "" } } };
table.rows.push_back(header);

sdlgui::RichTableRow row;
row.cells = { { { "alpha", {}, "" } }, { { "A longer cell that wraps internally", {}, "" } } };
table.rows.push_back(row);

rt->addTable(table);
```

Cell text is wrapped inside its column, participates in selection, and can contain styled runs/links. Column widths come from `columnWeights`; empty weights mean equal columns. Row/column spanning is not supported.

Selection: drag to select, Shift+click to extend, Ctrl+A / Ctrl+C / Esc when the view has focus. Call `setSelectable(false)` to disable it.

Current scope: paragraphs, headings, code blocks, quotes, list items, tables, mixed fonts/colors, automatic wrapping, links and selection/copy. Inline images and collapsible blocks are not implemented yet.

#### Image

Load and display an image. Requires `#define SDLGUI_USE_SDL_IMAGE` and SDL3_image library.

```cpp
#define SDLGUI_USE_SDL_IMAGE
#include "sdlgui.hpp"

auto* img = root.addChild<sdlgui::Image>();
img->pos(400, 40)->sz(200, 150);
if (!img->load("photo.png"))
{
    // Optionally provide your own texture (ownership is transferred):
    // img->setTexture(myTexture);
}
```

| Method | Returns | Description |
|--------|---------|-------------|
| `load(path)` | `bool` | Load image file (via IMG_Load) |
| `setTexture(SDL_Texture*)` | `void` | Use an existing texture (takes ownership) |
| `setColor(Color)` | `void` | Set modulation color |

**Note:** If `SDLGUI_USE_SDL_IMAGE` is not defined, using `Image` causes a compile error.

### Font System

#### Built-in Bitmap Font

Define `SDLGUI_USE_BUILTIN_FONT` before including `sdlgui.hpp` to use the built-in bitmap font. The font data is provided by `fontData.hpp`.

The glyph data is derived from **GNU Unifont** (https://unifoundry.com/unifont/), converted into the raw 8×16 / 16×16 bitmap tables used by this library.

```cpp
#define SDLGUI_USE_BUILTIN_FONT
#include "sdlgui.hpp"
```

The built-in font uses **two lookup tables** for Unicode BMP (codepoints 0x0000-0xFFFC):

| Array | Dimensions | Description |
|-------|-----------|-------------|
| `font8x16` | `[256][16]` | 8×16 glyphs for codepoints 0x0000-0x00FF (Latin-1) |
| `font16x16` | `[65533][32]` | 16×16 glyphs for codepoints 0x0000-0xFFFC (full BMP) |

**Lookup logic:**

1. If `cp < 256` and `font8x16[cp]` has non-zero content → use 8×16 glyph, width = 8
2. Else if `cp < 65533` → use 16×16 glyph, width = 16
3. Else → skip with width = 8

**Font data format:**

- 8×16: each glyph = 16 rows × 1 byte (8 pixels per row, bit7 = leftmost pixel)
- 16×16: each glyph = 16 rows × 2 bytes (left byte + right byte)

**Full-width vs half-width:** U+0020 (half-width space) → 8px. U+3000 (full-width space, CJK) → 16px. This is handled automatically by the lookup logic.

**Your `fontData.hpp` can provide both tables.** The bundled file already contains both `font8x16` and the full BMP `font16x16`; you can replace it with your own data using the same format.

#### Runtime Font Switching

The active font can be replaced at runtime:

```cpp
// Switch to a custom/loaded font
sdlgui::setFont(myFont);

// Switch back to the built-in Unifont (or the fallback font)
sdlgui::setFont(nullptr);

sdlgui::Font* f = sdlgui::currentFont();
uint64_t epoch = sdlgui::fontEpoch();
```

`Font` exposes the metrics and drawing entry points used by the library:

```cpp
class Font
{
public:
    virtual int  lineHeight() const;
    virtual int  ascent() const;
    virtual int  measure(const char* text) const = 0;
    virtual int  measureTo(const char* text, int bytePos) const = 0;
    virtual int  advance(uint32_t codepoint) const = 0;
    virtual void draw(SDL_Renderer*, const char* text,
                      int x, int y, Color color) = 0;
    //Optional: re-rasterize glyphs for Application::uiScale().
    virtual void setRasterScale(float scale) {}
};
```

For custom TTF/canvas fonts, override `setRasterScale(scale)` to rasterize glyphs at `fontSize * scale`, keep metrics in logical units, and draw back at logical size. `Application::setUiScale()` and `setFont()` call it automatically.

A quick adapter is available without subclassing:

```cpp
auto font = std::make_shared<sdlgui::CallbackFont>(
    [](const char* s) { return measureText(s); },
    [](const char* s, int n) { return measurePrefix(s, n); },
    [](uint32_t cp) { return advance(cp); },
    [](SDL_Renderer* r, const char* s, int x, int y, sdlgui::Color c) {
        drawText(r, s, x, y, c);
    },
    16, 16); // line height, ascent

sdlgui::setFont(font);
```

Switching fonts:

- clears cached text textures;
- bumps `fontEpoch()`, which invalidates `Label` layout caches;
- re-measures widgets with `setAutoHeight(true)` / `setAutoWidth(true)` and re-lays out their parents/boxes;
- refreshes TextBox/TextArea scrolling and marks widgets dirty;
- requests a redraw.

If a text widget keeps a fixed height chosen for the old font, it will clip on taller fonts. Opt in to content sizing where the height should follow the text:

```cpp
label->setAutoHeight(true);
vbox->setAutoHeight(true);  // and children autoH -> the box grows too
```

`CallbackFont` without measurement callbacks falls back to 8px per character. `setFontRenderer()` is kept as a legacy adapter when the built-in font is not compiled in; prefer `setFont()`.

**Note:** a widget's size is still explicit unless you enable `setAutoHeight()` / `setAutoWidth()`. For heights that must follow the font, turn auto-height on; otherwise a taller font can be clipped by the fixed rectangle.

#### fallback Behavior

Without a built-in font and without a custom `Font`, `drawText()` is a no-op — no text is rendered, and text width defaults to 8px per character.

### Theming

Apply a whole-window style in one call:

```cpp
win.setTheme(sdlgui::Theme::light());   // white theme
win.setTheme(sdlgui::Theme::dark());    // default dark theme

// Or apply to every window:
app.setTheme(sdlgui::Theme::light());

// Or to a widget subtree:
sdlgui::applyTheme(root, sdlgui::Theme::light());
```

`Theme` contains colors for windows, buttons, inputs, panels, lists, scrollbars, menus, popups, TextArea, sliders, title bars, message boxes, checkboxes and SplitView dividers. Individual widgets can still be overridden after applying a theme.

**Per-widget color APIs:**

| Widget | Setters |
|--------|---------|
| `Window` | `setBackgroundColor`, `setTitleBarColor`, `setTitleTextColor`, `setTitleButtonColors`, `setTitleCloseColor` |
| `Label` | `setTextColor` |
| `Button` | `setTextColor`, `setBgColor`, `setBorderColor` |
| `CheckBox` | `setTextColor`, `setCheckColor`, `setBoxColor`, `setBorderColor` |
| `TextBox` | `setBackgroundColor`, `setTextColor`, `setPlaceholderColor`, `setCursorColor`, `setSelectionColor`, `setBorderColor` |
| `TextArea` | `setBackgroundColor`, `setTextColor`, `setCursorColor`, `setSelectionColor`, `setBorderColor` |
| `ListBox` | `setBackgroundColor`, `setTextColor`, `setSelectionColor`, `setHoverColor`, `setBorderColor` |
| `ScrollView` | `setScrollbarColors(track, thumb)` |
| `Slider` | `setTrackColor`, `setFillColor`, `setThumbColor`, `setBorderColor` |
| `MenuBar` | `setBackgroundColor`, `setTextColor`, `setHoverColor`, `setPopupColor`, `setBorderColor`, `setItemHoverColor`, `setDisabledTextColor`, `setShortcutColor`, `setAccentColor` |
| `ContextMenu` | `setBackgroundColor`, `setTextColor`, `setHoverColor`, `setBorderColor`, `setDisabledTextColor`, `setSeparatorColor` |
| `DropDown` | `setBackgroundColor`, `setTextColor`, `setBorderColor`, `setArrowColor`, `setPopupColor`, `setSelectionColor` |
| `ComboBox` | same as DropDown plus `setButtonColor` |
| `MessageBox` | `setOverlayColor`, `setBackgroundColor`, `setBorderColor`, `setTextColor`, `setTitleColors`, `setButtonColors`, `applyTheme` |
| `SplitView` | `setDividerColor` |

### Color Reference

```cpp
struct Color {
    uint8_t r, g, b, a;
    
    static const Color Black;       // (0,0,0)
    static const Color White;       // (255,255,255)
    static const Color Red;         // (255,0,0)
    static const Color Green;       // (0,255,0)
    static const Color Blue;        // (0,0,255)
    static const Color Gray;        // (128,128,128)
    static const Color LightGray;   // (192,192,192)
    static const Color Transparent; // (0,0,0,0)
};
```

Colors can also be constructed inline: `sdlgui::Color(120, 30, 50)` or with initializer lists: `{120, 30, 50}`.

### Event Structures

```cpp
struct MouseEvent {
    int x, y;          // Position relative to the target widget
    int screenX, screenY; // Absolute screen position
    uint8_t button;    // SDL_BUTTON_LEFT, SDL_BUTTON_RIGHT, etc.
    uint8_t clicks;    // 1 for single-click, 2 for double-click
};

struct KeyEvent {
    SDL_Keycode key;   // SDLK_* constant
    uint16_t mod;      // KMOD_* modifier mask
    char text;         // Printable character (0 for control keys)
};
```

### Coding Style (约定)

| Category | Convention | Example |
|----------|-----------|---------|
| Classes/Structs | PascalCase | `class PushButton` |
| Functions/Vars/Members | camelCase | `setEnabled()`, `isVisible_` |
| Indentation | Allman | Braces on their own line |
| Comments | `//[TEXT]` no space after `//` | `//this is correct` |
| Files | camelCase | `sdlgui.hpp`, `fontData.hpp` |

### Building

#### Using Make (recommended)

```bash
make        # Release build
make debug  # Debug build
make clean  # Remove binary
```

The Makefile uses `pkg-config` to find SDL3. If `pkg-config` is unavailable, set `SDL3_CFLAGS` and `SDL3_LIBS` environment variables.

#### Manual Compilation

```bash
# Linux / macOS
g++ -std=c++14 $(pkg-config --cflags sdl3) -o demo demo.cc $(pkg-config --libs sdl3)

# Windows (MSYS2/MinGW)
g++ -std=c++14 $(pkg-config --cflags sdl3) -o demo demo.cc $(pkg-config --libs sdl3)
```

#### Without pkg-config

```bash
g++ -std=c++14 -I/path/to/SDL3/include -L/path/to/SDL3/lib -lSDL3 -mwindows -o demo demo.cc
```

### Examples

#### Minimal Application

```cpp
#define SDLGUI_IMPLEMENTATION
#define SDLGUI_USE_BUILTIN_FONT
#include "sdlgui.hpp"

int main()
{
    sdlgui::Application app;
    app.addWindow("Minimal", 400, 300);
    app.run();
    return 0;
}
```

#### Button with Custom Colors

```cpp
auto* btn = root.addChild<sdlgui::Button>();
btn->pos(20, 80)->sz(120, 30);
btn->setText("Red Button");
btn->setBgColor(
    sdlgui::Color(120, 20, 20),   // normal
    sdlgui::Color(180, 30, 30),   // hover
    sdlgui::Color(80, 10, 10)     // pressed
);
btn->onClick = [](sdlgui::MouseEvent&) {
    std::puts("Red button clicked!");
};
```

#### CheckBox with Toggle

```cpp
auto* cb = root.addChild<sdlgui::CheckBox>();
cb->pos(20, 130)->sz(160, 24);
cb->setText("Enable feature");
cb->setCheckColor(sdlgui::Color(0, 200, 0));
// CheckBox toggles itself on click, Space or Enter.
```

#### TextBox with Styling

```cpp
auto* tb = root.addChild<sdlgui::TextBox>();
tb->pos(20, 180)->sz(200, 28);
tb->setText("Hello");
tb->setBackgroundColor(sdlgui::Color(240, 240, 240));
tb->setTextColor(sdlgui::Color(30, 30, 30));
tb->setCursorColor(sdlgui::Color::Red);
```

#### Canvas Custom Drawing

```cpp
auto* cv = root.addChild<sdlgui::Canvas>();
cv->pos(20, 240)->sz(300, 200);
cv->onDraw = [](SDL_Renderer* r, const sdlgui::Rect& a)
{
    // Grid
    SDL_SetRenderDrawColor(r, 60, 60, 60, 255);
    for (int x = 0; x < a.w; x += 40)
        SDL_RenderLine(r, a.x + x, a.y, a.x + x, a.y + a.h);
    for (int y = 0; y < a.h; y += 40)
        SDL_RenderLine(r, a.x, a.y + y, a.x + a.w, a.y + y);

    // Diagonal
    SDL_SetRenderDrawColor(r, 200, 100, 50, 255);
    SDL_RenderLine(r, a.x, a.y, a.x + a.w, a.y + a.h);
};
```

### Known Limitations

- Multi-window focus/event routing is implemented, but multi-window apps are still not exhaustively tested
- Text selection exists in TextBox and TextArea; ComboBox does not have a selection model yet
- TextArea has no undo/redo, syntax highlighting, or large-file buffer optimization yet; Tab focus traversal is opt-in
- No grid layout, docking, or CSS-style flex; anchors, HBox/VBox and SplitView are the supported layout primitives
- No fullscreen API; windows are windowed only
- The built-in font is a fixed 16px bitmap; runtime `Font` objects can change metrics/drawing, and logical presentation handles HiDPI coordinates, but there is no automatic UI scaling/kerning/ligature handling
- `Label` wrapping is character-level; `TextArea` uses word-aware wrapping
- `RichTextView` has no inline images, collapsible blocks, or table row/column spanning yet
- Legacy `setFontRenderer()` fonts are measured as 8px per character; use `setFont()` for real metrics
- Redraw-on-demand is whole-window: no partial dirty rectangles
- Inline IME composition is drawn by TextBox/TextArea; the caret rect is also reported to `SDL_SetTextInputArea` so the candidate window follows it
- No accessibility support
- `SDLGUI_IMPLEMENTATION` must be defined in exactly one translation unit

### License

BSD 2-Clause License. See [LICENSE](LICENSE).

---

## 中文版

### 概述

**sdlgui** 是一个基于 SDL3 的轻量级 C++14 GUI 库。采用单头文件（stb 风格）、保留模式控件树、简洁可链式调用的 API。

**核心特性：**

- 单头文件：`sdlgui.hpp` — 即拿即用
- 保留模式控件树，自动裁剪
- 通过 `fontData.hpp` 支持内置 Unifont 位图字体（含 Unicode BMP）
- 通过 `setFont()` / `Font` metrics 支持运行时切换字体，并自动失效缓存和布局
- 可选的自动内容尺寸（`setAutoHeight` / `setAutoWidth`），让文本控件跟随字体度量变化
- `addChild<T>()` 模板 — 无需手动 `make_unique` + `move`
- 可链式调用的位置/大小 API：`pos()->sz()`
- 内置深色/浅色主题，并提供逐控件颜色 API
- `RichTextView` 支持富文本 run、代码块、引用和链接
- 自定义标题栏（最小化、最大化、关闭按钮）
- TextBox / TextArea / ComboBox 编辑：光标、剪贴板（TextBox 另支持占位符、密码掩码和最大长度；TextBox/TextArea 编辑键位可复写；输入法候选窗跟随光标）
- 按需重绘：空闲窗口跳过 Clear / Draw / Present
- 通过 SDL3 支持窗口（系统标题栏或自定义标题栏）

### 环境要求

- C++14 编译器
- SDL3 **3.2.0 或更高**（使用 `SDL_RenderClipEnabled`、`SDL_GetTextureSize`）
- （可选）`pkg-config` 用于简化编译配置

### 快速开始

```cpp
#define SDLGUI_IMPLEMENTATION
#define SDLGUI_USE_BUILTIN_FONT
#include "sdlgui.hpp"

int main()
{
    sdlgui::Application app;
    auto& win = app.addWindow("Hello", 800, 600);

    auto* btn = win.root().addChild<sdlgui::Button>();
    btn->pos(20, 80)->sz(120, 30);
    btn->setText("点我");
    btn->onClick = [](sdlgui::MouseEvent&)
    {
        printf("点击！\n");
    };

    app.run();
    return 0;
}
```

编译：

```bash
g++ -std=c++14 $(pkg-config --cflags sdl3) -o demo demo.cc $(pkg-config --libs sdl3)
```

### 文件结构

```
sdlgui/
├── sdlgui.hpp          # 主库（单头文件，stb 风格）
├── fontData.hpp        # 内置位图字体数据（8x16 + 全 BMP 16x16）
├── examples/
│   ├── demo.cc         # 控件展示
│   └── counter.cc      # 最小计数器示例
├── LICENSE             # BSD 2-Clause
├── Makefile            # 构建配置
└── readme.md           # 文档（英文 + 中文）
```

### 架构

#### 控件树

sdlgui 采用保留模式架构。所有 UI 元素继承自 `Widget`，以树形结构管理子控件：

```
Application
  └── Window（持有 SDL_Window + SDL_Renderer）
       └── root Widget（根控件）
            ├── Label（文本标签）
            ├── Button（按钮）
            ├── TextBox（文本框）
            ├── CheckBox（复选框）
            ├── ScrollView（滚动视图）
            │    ├── Canvas（自定义画布）
            │    └── Label
            └── ...
```

- 每个窗口有一个根控件。
- 父控件通过 `std::unique_ptr` 持有子控件所有权。
- 子控件通过原始指针引用父控件。
- 坐标为**相对父控件**，绝对位置通过 `absoluteX()` / `absoluteY()` 递归计算。
- Z 序由添加顺序决定：后添加的控件绘制在上层。

#### 事件流

```
SDL_PollEvent()
  → Application::run() 判断事件类型
    → handleMouseEvent() / handleKeyEvent()
      → Window::handleTitleBarEvent()    自定义标题栏按钮处理
      → hitTest()                         递归查找最上层命中控件
      → target->onMouseDown / onClick / onKeyDown 等 std::function 回调
```

事件精确发送给一个控件（最上层的命中控件），不冒泡。每个控件将回调存储为 `std::function<void(...)>` 成员：

```cpp
button->onClick = [](MouseEvent& e) { /* ... */ };
```

#### 坐标系

- `x()`、`y()`、`width()`、`height()` — 相对于父控件
- `absoluteX()`、`absoluteY()` — 沿父链递归计算
- `absoluteRect()` — 绝对坐标矩形
- 命中检测和渲染内部使用绝对坐标

#### 按需重绘

只在需要时重绘。属性变化会通过 `markDirty()` 向上传递到根控件；控件树干净时 `Window::render()` 会跳过整帧（包括 `SDL_RenderClear` 和 `SDL_RenderPresent`）。文本光标在闪烁相位变化时置脏，菜单、右键菜单、消息框、标题栏悬停等 overlay 状态变化会显式请求重绘。

带 `onDraw` 的 `Canvas` 默认连续重绘，保证动画行为不变；静态画布可调用 `setContinuousRedraw(false)`。自定义动画可以调用 `Widget::requestRedraw()` / `Window::requestRedraw()`。定义 `SDLGUI_ALWAYS_REDRAW` 可恢复旧的每帧重绘行为。

### 控件参考

#### Widget（基类）

所有 UI 元素继承自 `Widget`，提供：

- 位置/大小管理
- 父子控件树操作（`addChild<T>()`、`addChild(unique_ptr)`、`removeChild()`）
- 事件回调（全部为 `std::function`，C++11 兼容）
- 脏标记与 `requestRedraw()`
- `addChild<T>(args...)` 模板，类型安全地创建子控件

**事件回调：**

| 回调 | 签名 | 触发时机 |
|------|------|---------|
| `onMouseDown` | `void(MouseEvent&)` | 鼠标按下 |
| `onMouseUp` | `void(MouseEvent&)` | 鼠标释放 |
| `onMouseMove` | `void(MouseEvent&)` | 鼠标移动 |
| `onClick` | `void(MouseEvent&)` | 按下+释放且在范围内 |
| `onMouseEnter` | `void(MouseEvent&)` | 鼠标进入控件区域 |
| `onMouseLeave` | `void(MouseEvent&)` | 鼠标离开控件区域 |
| `onLongPress` | `void(MouseEvent&)` | 触摸长按（不会再触发点击）；未设置且存在 `onContextMenu` 时，长按会打开上下文菜单 |
| `onKeyDown` | `void(KeyEvent&)` | 按键按下 |
| `onKeyUp` | `void(KeyEvent&)` | 按键释放 |
| `onFocus` | `void()` | 控件获得键盘焦点 |
| `onBlur` | `void()` | 控件失去键盘焦点 |
| `onContextMenu` | `void(MouseEvent&, ContextMenu&)` | 右键点击——填充菜单项 |

**链式方法：**

```cpp
Widget* pos(int x, int y);   // 调用 setPosition()
Widget* sz(int w, int h);    // 调用 setSize()
Widget* vis(bool v);         // 调用 setVisible()
Widget* ena(bool v);         // 调用 setEnabled()
Widget* anch(int flags);     // 调用 setAnchors()
Widget* stretch(float w);    // 调用 setStretch()
Widget* fillCross(bool on);  // 调用 setFillCross()
Widget* autoH(bool on);      // 调用 setAutoHeight()
Widget* autoW(bool on);      // 调用 setAutoWidth()
```

| 方法 | 返回 | 说明 |
|------|------|------|
| `setEnabled(bool)` | `void` | 启用/禁用控件交互 |
| `enabled()` | `bool` | 检查控件是否启用 |
| `ena(bool)` | `Widget*` | 链式 setEnabled |
| `removeChild(Widget*)` | `void` | 移除并销毁直接子控件 |
| `detachChild(Widget*)` | `unique_ptr<Widget>` | 移除直接子控件但不销毁 |
| `reparent(Widget*)` | `void` | 把当前控件移动到另一个父节点（保留位置/尺寸） |
| `setFocusable(bool)` | `void` | 开启 Tab 遍历时加入焦点顺序 |
| `focusable()` | `bool` | 是否参与键盘焦点 |
| `setAutoHeight(bool)` / `setAutoWidth(bool)` | `void` | 让该方向尺寸由内容决定 |
| `preferredHeight(width)` / `preferredWidth(height)` | `int` | 内容尺寸建议值（默认返回当前尺寸） |
| `requestRedraw()` | `void` | 请求所属窗口再绘制一帧 |

自动尺寸默认关闭，`sz()` 的固定矩形语义保持不变。`Label`、`Button`、`CheckBox` 和 `Box` 实现了 `preferredHeight()` / `preferredWidth()`；字体切换时会重新测量自动尺寸控件，切换到行高更大的字体也不会截断文字。

禁用控件不响应点击（hitTest 失败），渲染时显示灰色。禁用父控件会连带禁用所有子控件。

#### 布局

控件默认使用绝对定位；布局是可选功能，可以和手动坐标混用。

**锚点**

```cpp
auto* editor = root.addChild<sdlgui::Widget>();
editor->pos(160, 40)->sz(400, 300);
editor->setAnchors(sdlgui::AnchorLeft | sdlgui::AnchorTop |
                   sdlgui::AnchorRight | sdlgui::AnchorBottom);
```

`setAnchors(flags)` 会按当前几何位置记录到父控件四边的边距。Left+Right 拉伸宽度，Top+Bottom 拉伸高度，单独 Right/Bottom 则保持到该边的距离。也可以显式指定边距：

```cpp
panel->setAnchors(sdlgui::AnchorLeft | sdlgui::AnchorTop, 8, 8, 0, 0);
```

| 方法 | 返回 | 说明 |
|------|------|------|
| `setAnchors(flags)` | `void` | 锚定到边，按当前位置记录边距 |
| `setAnchors(flags,l,t,r,b)` | `void` | 使用显式边距锚定 |
| `setAnchorMargins(l,t,r,b)` | `void` | 修改边距并重新布局 |
| `anchors()` | `int` | 当前锚点标志 |
| `relayout()` | `void` | 强制重新布局子控件 |
| `anch(flags)` | `Widget*` | 链式 setAnchors |

**HBox / VBox**

```cpp
auto* row = root.addChild<sdlgui::HBox>();
row->pos(0, 0)->sz(600, 36);
row->setPadding(4);
row->setSpacing(6);

auto* open = row->addChild<sdlgui::Button>();
open->sz(80, 28);
open->txt("打开");

auto* path = row->addChild<sdlgui::TextBox>();
path->stretch(1.0f);       // 占满剩余宽度
path->fillCross(true);     // 填满行高
```

- `HBox` 从左到右排列，`VBox` 从上到下排列。
- 未设置 stretch 的子控件保持当前主轴尺寸。
- `setStretch(weight)` 按权重分配剩余主轴空间。
- `setFillCross(true)` 填满交叉轴；否则保持自身尺寸。
- 不可见子控件会被跳过。
- HBox/VBox 管理直接子控件，因此这些子控件上的锚点会被忽略。

**SplitView**

```cpp
auto* split = root.addChild<sdlgui::SplitView>();
split->setAnchors(sdlgui::AnchorLeft | sdlgui::AnchorTop |
                  sdlgui::AnchorRight | sdlgui::AnchorBottom);
split->setOrientation(sdlgui::SplitView::Orientation::SideBySide);
split->setSplit(0.3f);
split->setDividerWidth(6);

auto* sidebar = split->addChild<sdlgui::VBox>(); // 第一个可见子控件
auto* editor  = split->addChild<sdlgui::VBox>(); // 第二个可见子控件
```

`SideBySide` 左右分栏，`Stacked` 上下分栏。拖动分隔条调整比例；`setMinPaneSize()` 限制两侧最小尺寸。`setDividerHitWidth(px)` 可将视觉宽度与拖动命中宽度分开（`0` = `max(dividerWidth, 6)`）。

**Drawer（抽屉）**

带遮罩的全尺寸覆盖面板，适合移动端侧边栏。作为 root 的最后一个子控件添加，确保覆盖其他控件：

```cpp
auto* drawer = root.addChild<sdlgui::Drawer>();
drawer->setAnchors(sdlgui::AnchorLeft | sdlgui::AnchorTop |
                   sdlgui::AnchorRight | sdlgui::AnchorBottom);
drawer->setPanelWidth(260);
drawer->setSide(sdlgui::Drawer::Side::Left);

auto* list = drawer->panel()->addChild<sdlgui::ListBox>();
list->pos(10, 10)->sz(240, 400);

drawer->open();    // 或 close() / toggle()
drawer->onOpenChanged = [](bool open) { /* ... */ };
```

点击遮罩会关闭抽屉，不会误触下面的控件。面板内容是普通控件树（`drawer->panel()`）。调用 `drawer->setCloseButtonVisible(true)` 会在面板右上角显示内置关闭按钮。`setScrimColor()` / `setPanelColor()` 的 alpha 会生效（遮罩会与背后内容混合）。

#### Label（文本标签）

显示静态文本。

```cpp
auto* label = root.addChild<sdlgui::Label>();
label->pos(20, 40)->sz(200, 20);
label->setText("你好，世界！");
label->setTextColor(sdlgui::Color::Green);
```

| 方法 | 返回 | 说明 |
|------|------|------|
| `setText(string)` | `void` | 设置文本内容 |
| `setTextColor(Color)` | `void` | 设置文本颜色 |
| `setTextAlign(RichTextAlign)` | `void` | 按行左/中/右对齐 |
| `txt(string)` | `Widget*` | 链式 setText |
| `clr(Color)` | `Widget*` | 链式 setTextColor |

#### Button（按钮）

可点击按钮，三种视觉状态：Normal（正常）、Hover（悬停）、Pressed（按下）。

```cpp
auto* btn = root.addChild<sdlgui::Button>();
btn->pos(20, 80)->sz(120, 30);
btn->setText("点击");
btn->setBgColor({60,60,60}, {80,80,80}, {40,40,40});  // 正常, 悬停, 按下
btn->onClick = [](sdlgui::MouseEvent&) { /* ... */ };
```

| 方法 | 返回 | 说明 |
|------|------|------|
| `setText(string)` | `void` | 设置按钮文本 |
| `setTextColor(Color)` | `void` | 设置文本颜色 |
| `setBgColor(Color,Color,Color)` | `void` | 设置正常/悬停/按下颜色 |
| `txt(string)` | `Widget*` | 链式 setText |
| `clr(Color)` | `Widget*` | 链式 setTextColor |
| `bg(Color,Color,Color)` | `Widget*` | 链式 setBgColor |
| `onClick` | 回调 | 鼠标在范围内释放时触发 |

按钮通过内部注册的 `onMouseEnter`、`onMouseLeave`、`onMouseDown`、`onMouseUp` 自动管理状态。

#### CheckBox（复选框）

带文本标签的开关控件。

```cpp
auto* cb = root.addChild<sdlgui::CheckBox>();
cb->pos(20, 130)->sz(160, 24);
cb->setText("启用选项");
```

CheckBox 点击时自动切换；获得焦点后 Space/Enter 也可以切换，无需重写 `onClick`。

| 方法 | 返回 | 说明 |
|------|------|------|
| `setText(string)` | `void` | 设置标签文本 |
| `setChecked(bool)` | `void` | 设置选中状态 |
| `setTextColor(Color)` | `void` | 设置文本颜色 |
| `setCheckColor(Color)` | `void` | 设置勾选颜色 |
| `txt(string)` | `Widget*` | 链式 setText |
| `clr(Color)` | `Widget*` | 链式 setTextColor |
| `checkClr(Color)` | `Widget*` | 链式 setCheckColor |

#### TextBox（文本框）

单行文本输入框，支持光标、文本编辑和自动滚动。

```cpp
auto* tb = root.addChild<sdlgui::TextBox>();
tb->pos(20, 180)->sz(200, 28);
tb->setPlaceholder("在此输入");
tb->setMaxLength(32);

auto* pw = root.addChild<sdlgui::TextBox>();
pw->pos(20, 216)->sz(200, 28);
pw->setPassword(true);
pw->setPlaceholder("密码");
```

| 方法 | 返回 | 说明 |
|------|------|------|
| `setText(string)` | `void` | 设置文本内容，光标移至末尾 |
| `setTextColor(Color)` | `void` | 设置文字颜色 |
| `setBackgroundColor(Color)` | `void` | 设置背景颜色 |
| `setCursorColor(Color)` | `void` | 设置光标颜色 |
| `setSelectionColor(Color)` | `void` | 设置选择区背景色 |
| `setPlaceholder(string)` | `void` | 文本为空时显示的占位符 |
| `setPlaceholderColor(Color)` | `void` | 设置占位符颜色 |
| `setContentInsets(l,t,r,b)` | `void` | 文本/光标/选区的内边距 |
| `setBorderVisible(bool)` / `setBackgroundVisible(bool)` | `void` | 开关边框/背景绘制 |
| `setMaxLength(int)` | `void` | 限制 N 个 Unicode 码点；`0` 表示不限制 |
| `setPassword(bool)` | `void` | 以掩码显示文本 |
| `setPasswordChar(char)` | `void` | 设置掩码字符（默认 `*`） |
| `insertText(string)` | `void` | 插入文本，替换选择区并遵守最大长度 |
| `scrollByPixels(int)` | `bool` | 手动水平滚动文本；返回是否发生滚动 |
| `adjustScroll()` | `void` | 确保光标可见（自动调用） |
| `cursorPosition()` | `int` | 获取光标字节位置 |
| `hasSelection()` | `bool` | 是否有选中文本 |
| `selectedText()` | `string` | 获取选中文本 |
| `selectAll()` / `clearSelection()` | `void` | 全选 / 取消选择 |
| `txt(string)` | `Widget*` | 链式 setText |
| `clr(Color)` | `Widget*` | 链式 setTextColor |
| `bgClr(Color)` | `Widget*` | 链式 setBackgroundColor |
| `ph(string)` / `maxLen(int)` / `pwd(bool)` | `Widget*` | 链式占位符 / 最大长度 / 密码 |

**键盘控制：**

| 键 | 动作 |
|----|------|
| 任意可打印字符 | 替换选择区，或在光标处插入 |
| 退格键 / Delete | 删除选择区，或删除一个字符 |
| 左/右方向键 | 移动光标并取消选择 |
| Shift+左/右/Home/End | 扩展选择区 |
| Home / End | 光标移至开头 / 末尾 |
| Ctrl+A | 全选 |
| Ctrl+C | 复制选择区（无选择时无操作） |
| Ctrl+X | 剪切选择区（无选择时无操作） |
| Ctrl+V | 在光标处粘贴，替换选择区 |
| Esc | 取消选择 |

所有编辑快捷键都支持按控件复写，见下文 **键位复写**。

TextBox 在文本超出可见区域时自动水平滚动，保持光标可见。横向滚轮/触摸板手势或 Shift+滚轮可手动水平滚动。密码模式按 Unicode 字符显示掩码，但 `text()` 和 `selectedText()` 仍返回真实文本。

#### ScrollView（滚动视图）

用于显示比可见区域更大的内容的滚动容器。

```cpp
auto* sv = root.addChild<sdlgui::ScrollView>();
sv->pos(20, 240)->sz(300, 200);
sv->setContentSize(600, 400);
// 向 sv 添加子控件——它们会被裁剪并支持滚动
```

| 方法 | 返回 | 说明 |
|------|------|------|
| `setContentSize(w, h)` | `void` | 设置虚拟内容总尺寸 |
| `setScrollOffset(x, y)` | `void` | 设置滚动偏移（自动限制边界） |
| `scrollbarV(bool)` | `void` | 显示/隐藏垂直滚动条（默认隐藏） |
| `scrollbarH(bool)` | `void` | 显示/隐藏水平滚动条（默认隐藏） |
| `sbV(bool)` | `ScrollView*` | 链式 scrollbarV |
| `sbH(bool)` | `ScrollView*` | 链式 scrollbarH |

ScrollView 通过 `SDL_SetRenderClipRect` 将子控件裁剪到自身边界内。渲染时内容会临时偏移。命中检测已正确处理滚动偏移。

滚动条**默认不显示**——需要调用 `scrollbarV(true)` 或 `scrollbarH(true)` 才出现（且仅在内容超出可见区域时绘制）。鼠标滚轮不受滚动条可见性影响，始终可滚。

#### Canvas（画布）

通过回调暴露原始 SDL_Renderer 给用户进行自定义绘制。

```cpp
auto* cv = root.addChild<sdlgui::Canvas>();
cv->pos(20, 240)->sz(300, 200);
cv->onDraw = [](SDL_Renderer* r, const sdlgui::Rect& area)
{
    SDL_SetRenderDrawColor(r, 255, 0, 0, 255);
    SDL_RenderLine(r, area.x, area.y, area.x + area.w, area.y + area.h);
};
```

**`onDraw` 回调签名：** `void(SDL_Renderer*, const Rect& clipArea)`

`clipArea` 参数包含画布的绝对位置和尺寸，与调用 `onDraw` 前设置的裁剪矩形一致。

**重绘策略：** 设置了 `onDraw` 的 Canvas 默认每帧重绘，以保持动画行为；静态内容可调用 `setContinuousRedraw(false)`，之后在内容变化时调用 `requestRedraw()` / `markDirty()`。`examples/demo.cc` 的静态网格/圆就是这样处理的。

#### Window（窗口）

代表一个操作系统窗口，拥有自己的 SDL_Window 和 SDL_Renderer。

```cpp
auto& win = app.addWindow("我的应用", 800, 600);
```

**标题栏模式：**

| 模式 | 说明 |
|------|------|
| `TitleBarMode::System` | 系统原生标题栏（默认） |
| `TitleBarMode::Custom` | sdlgui 自绘标题栏 |
| `TitleBarMode::Hidden` | 无标题栏，全客户区 |

```cpp
win.setTitleBarMode(sdlgui::Window::TitleBarMode::Custom);
win.setBackgroundColor(sdlgui::Color(30, 30, 40));
```

**自定义标题栏按钮：**

| 按钮 | 颜色（默认） | 颜色（悬停） | 动作 |
|------|-------------|-------------|------|
| 最小化 `_` | 透明 | 浅灰 | `SDL_MinimizeWindow` |
| 最大化 `□` | 透明 | 浅灰 | `SDL_MaximizeWindow` / `SDL_RestoreWindow` |
| 关闭 `X` | 透明 | 红色背景 | `SDL_PushEvent(SDL_EVENT_QUIT)` |

| 方法 | 返回 | 说明 |
|------|------|------|
| `setTitle(const char*)` | `void` | 设置窗口标题 |
| `setTitleBarMode(TitleBarMode)` | `void` | 切换标题栏模式 |
| `setTitleBarHeight(int)` | `void` | 设置自定义标题栏高度 |
| `setTitleIconTexture(SDL_Texture*)` | `void` | 用纹理设置图标（接管所有权） |
| `setTitleIconSurface(SDL_Surface*)` | `bool` | 用 Surface 设置图标（接管所有权） |
| `setTitleIconMemory(data, size)` | `bool` | 用内嵌 PNG/BMP 字节设置图标 |
| `setTitleIconPixels(w, h, rgba)` | `bool` | 用 RGBA 像素数据设置图标 |
| `clearTitleIcon()` | `void` | 移除标题栏图标 |
| `setTitleIconSize(int)` | `void` | 图标框尺寸；`0` = 自动 |
| `setTitleIconPadding(left, gap)` | `void` | 左边距和与标题的间距 |
| `setWindowIconMemory(data, size)` | `bool` | 用 PNG/BMP 字节设置系统/任务栏图标 |
| `setBackgroundColor(Color)` | `void` | 设置窗口背景颜色 |
| `setResizable(bool)` | `void` | 运行时切换窗口可缩放性 |
| `requestRedraw()` | `void` | 强制再渲染一帧 |
| `needsRedraw()` | `bool` | 下一次 `render()` 是否真的会绘制 |
| `valid()` | `bool` | SDL 窗口/渲染器是否创建成功 |
| `root()` | `Widget&` | 获取根控件，用于添加子控件 |
| `handle()` | `SDL_Window*` | 获取原生 SDL 窗口句柄 |
| `renderer()` | `SDL_Renderer*` | 获取原生 SDL 渲染器 |

无边框的 `Custom` / `Hidden` 窗口使用原生边缘命中测试：鼠标进入外侧 6px（边和角）会显示系统缩放光标，拖动即可缩放。调用 `setResizable(false)` 可关闭。运行时切换标题栏模式会自动安装或移除命中测试。

自绘标题栏支持左侧图标。单文件分发时把 PNG 编进程序，不要附带 `.ico`：

```bash
xxd -i app_icon.png > app_icon.h
```

```cpp
win.setTitleBarMode(sdlgui::Window::TitleBarMode::Custom);
win.setTitleIconMemory(app_icon_png, app_icon_png_len);
win.setWindowIconMemory(app_icon_png, app_icon_png_len); // 任务栏/Dock

win.onTitleIconClick = [] {
    // 可选：打开应用菜单
};
```

`setTitleIconMemory()` 不需要 SDL3_image，支持 PNG/BMP；纹理/Surface 接口会接管所有权，内存数据只在调用期间读取。

#### Application（应用程序）

管理窗口和主事件循环。

```cpp
sdlgui::Application app;
auto& win = app.addWindow("标题", 800, 600);
app.run();  // 阻塞至退出
```

| 方法 | 返回 | 说明 |
|------|------|------|
| `addWindow(title, w, h)` | `Window&` | 创建新窗口 |
| `run()` | `void` | 进入主事件循环（阻塞） |
| `quit()` | `void` | 请求退出事件循环 |
| `isRunning()` | `bool` | 检查事件循环是否运行中 |
| `processEvents()` | `void` | 泵送一帧事件（非阻塞） |
| `setFocus(Widget*)` | `void` | 设置键盘焦点到指定控件 |
| `focused()` | `Widget*` | 获取当前焦点控件 |
| `valid()` | `bool` | `SDL_Init` 是否成功 |
| `setFPS(fps)` | `void` | 设置目标帧率（0 = 不限，默认 60） |
| `setUiScale(scale)` | `void` | 逻辑 UI 缩放；通过 `Font::setRasterScale` 通知当前字体重新光栅化 |
| `fps()` | `int` | 获取当前目标帧率 |
| `setTouchMode(bool)` | `void` | 强制启用移动/触摸行为 |
| `setTouchTargetPadding(px)` | `void` | 触摸模式下扩大命中区域 |
| `setKeyboardInset(px)` | `void` | 为软键盘预留底部空间 |

**回调：**

| 回调 | 签名 | 说明 |
|------|------|------|
| `onUserEvent` | `void(const SDL_Event&)` | 未处理 SDL 事件的钩子（在 processEvents 末尾派发） |

事件循环按目标帧率轮询事件（默认 60 FPS），但空闲窗口不会重绘。`setFPS(30)` 可降低轮询/动画节奏；`setFPS(0)` 则去掉帧延迟。`processEvents()` 是公开的，可用于自定义事件循环（阻塞对话框内部也使用它）。

#### 键盘焦点

- `Tab` / `Shift+Tab` 焦点遍历是**可选功能**，默认关闭；调用 `app.setTabTraversal(true)` 开启。开启后在可聚焦控件间切换（TextBox、TextArea、ComboBox、Button、CheckBox、ListBox、Slider；自定义控件用 `setFocusable(true)` 加入）
- 仅在键盘焦点时绘制细蓝色焦点环；鼠标点击不绘制（TextBox 无焦点边框，靠光标指示焦点）
- Button：`Enter` / `Space` 触发；CheckBox：`Enter` / `Space` 切换
- ListBox：`Up` / `Down` 改变选中项
- Slider：`Left` / `Right` 步进；`Home` / `End` 跳到最小/最大
- `Esc` 关闭右键菜单 / 菜单栏下拉，并关闭消息框；`Enter` 接受消息框默认按钮

#### ListBox（列表框）

单选列表控件。

```cpp
auto* lb = root.addChild<sdlgui::ListBox>();
lb->pos(20, 400)->sz(200, 150);
lb->addItem("选项 A");
lb->addItem("选项 B");
lb->setSelectedIndex(0);
lb->onItemSelected = [](int index) { /* ... */ };
```

| 方法 | 返回 | 说明 |
|------|------|------|
| `addItem(string)` | `void` | 添加条目 |
| `removeItem(index)` | `void` | 删除指定条目 |
| `clear()` | `void` | 清空所有条目 |
| `selectedIndex()` | `int` | 获取选中索引（-1 表示无） |
| `selectedItem()` | `string&` | 获取选中条目的文本 |
| `onItemSelected` | 回调 | 点击条目时触发 |
| `onItemActivated` | 回调 | 双击或回车时触发 `void(int)` |

#### Slider（滑块）

水平浮点数值滑块。

```cpp
auto* sl = root.addChild<sdlgui::Slider>();
sl->pos(20, 560)->sz(200, 24);
sl->setRange(0.f, 100.f);
sl->setValue(50.f);
sl->onValueChanged = [](float val) { /* ... */ };
```

| 方法 | 返回 | 说明 |
|------|------|------|
| `setRange(min, max)` | `void` | 设置范围（float） |
| `setValue(val)` | `void` | 设置当前值（float） |
| `value()` | `float` | 获取当前值 |
| `onValueChanged` | 回调 | 拖动时触发 `void(float)` |
| `onDragChanged` | 回调 | 拖拽开始 (true) / 结束 (false) 时触发 |

#### DropDown（下拉框）

点击展开选择列表。

```cpp
auto* dd = root.addChild<sdlgui::DropDown>();
dd->pos(20, 600)->sz(160, 24);
dd->addItem("选项 A");
dd->addItem("选项 B");
dd->setSelectedIndex(0);
dd->onItemSelected = [](int index) { /* ... */ };
```

| 方法 | 返回 | 说明 |
|------|------|------|
| `addItem(string)` | `void` | 添加条目 |
| `clear()` | `void` | 清空 |
| `selectedIndex()` | `int` | 获取选中索引 |
| `setSelectedIndex(int)` | `void` | 设置选中项 |
| `onItemSelected` | 回调 | 选中时触发 |

展开列表作为覆盖层绘制在所有普通控件之上，后添加的重叠兄弟控件也不会盖住它。点击列表优先响应；点击外部只关闭列表，不会误触下面的控件。下方空间不足时列表会向上展开。

#### ComboBox（组合框）

可编辑文本框 + 下拉列表。

```cpp
auto* cb = root.addChild<sdlgui::ComboBox>();
cb->pos(20, 640)->sz(200, 24);
cb->addItem("苹果");
cb->addItem("香蕉");
cb->setSelectedIndex(0);
cb->onTextChanged = [](const std::string& text) { /* ... */ };
```

文本区可编辑：输入、退格/Delete、左右/Home/End、Ctrl+V 均可用；点击文本区会按列定位光标。下拉展开后点击外部会自动收起。

| 方法 | 返回 | 说明 |
|------|------|------|
| `addItem(string)` | `void` | 添加下拉项 |
| `clear()` | `void` | 清空项目和文本 |
| `selectedIndex()` | `int` | 获取选中索引 |
| `setSelectedIndex(int)` | `void` | 选中并同步到文本框 |
| `setText(string)` / `text()` | `void` / `string` | 设置/获取文本 |
| `onItemSelected` | 回调 | `void(int)`，选择项目时触发 |
| `onTextChanged` | 回调 | `void(const string&)`，文本编辑时触发 |
| `isPopupOpen()` / `closePopup()` | `bool` / `void` | 查询/关闭下拉 |

#### ContextMenu（右键菜单）

右键弹出菜单，支持多个项目。

```cpp
widget->onContextMenu = [](sdlgui::MouseEvent&, sdlgui::ContextMenu& menu)
{
    menu.addItem("复制",   []() { /* ... */ });
    menu.addItem("粘贴", []() { /* ... */ });
    menu.addSeparator();
    menu.addItem("删除", []() { /* ... */ });
};
```

| 方法 | 返回 | 说明 |
|------|------|------|
| `addItem(text, callback)` | `void` | 添加可点击的菜单项 |
| `addSeparator()` | `void` | 添加分隔线 |
| `clear()` | `void` | 清空所有项目 |
| `show(x, y)` | `void` | 在屏幕位置显示菜单 |
| `dismiss()` | `void` | 隐藏菜单 |
| `onDismiss` | 回调 | 菜单关闭时触发 |

点击菜单外自动关闭。每个控件可有自己的 `onContextMenu` 回调，提供不同的菜单项。行高跟随当前字体（`itemHeight()`，最小 22px），大字体不会挤在一起；菜单打开期间切换字体会自动重新适配。

#### MenuBar（菜单栏）

横向菜单栏，支持点击弹出下拉菜单，含子菜单项、分隔线和快捷键提示。

```cpp
auto* mb = root.addChild<sdlgui::MenuBar>();
mb->pos(0, 0)->sz(800, 24);

int file = mb->addMenu("文件");
mb->addItem(file, "新建",   []() { /* ... */ }, "Ctrl+N");
mb->addSeparator(file);
mb->addItem(file, "打开",   []() { /* ... */ }, "Ctrl+O");
mb->addItem(file, "保存",   []() { /* ... */ }, "Ctrl+S");

int edit = mb->addMenu("编辑");
mb->addItem(edit, "撤销",   []() { /* ... */ }, "Ctrl+Z");
mb->addItem(edit, "重做",   []() { /* ... */ }, "Ctrl+Shift+Z");
```

| 方法 | 返回 | 说明 |
|------|------|------|
| `addMenu(title)` | `int` | 添加菜单标题，返回菜单索引 |
| `addItem(menuIdx, text, onClick, shortcut)` | `int` | 添加菜单项，可选快捷键提示 |
| `addSeparator(menuIdx)` | `int` | 在菜单中添加分隔线 |
| `clearMenu(menuIdx)` | `void` | 清空指定菜单的所有项目 |
| `clearAll()` | `void` | 删除所有菜单 |
| `closeAll()` | `void` | 关闭已打开的下拉菜单 |
| `onItemSelected` | 回调 | 任意条目被点击时触发 `(menuIdx, itemIdx)` |

**行为说明：**
- 点击菜单标题展开下拉菜单；点击另一个标题自动切换。
- 菜单打开时，悬停到另一个标题自动切换。
- 点击菜单项或在下拉菜单外点击自动关闭。
- 快捷键提示在下拉菜单中右对齐。
- 默认高度 24px。

#### MessageBox（消息框）

带图标、消息文本和按钮的模式对话框。作为覆盖层渲染在所有窗口内容之上。

**非阻塞（回调版）：**

```cpp
win.showMessageBox("错误", "文件未找到！",
    sdlgui::MessageBoxType::Error,
    sdlgui::MessageBoxButtons::OKCancel,
    [](sdlgui::MessageBoxResult r) {
        if (r == sdlgui::MessageBoxResult::OK) { /* ... */ }
    });
```

**阻塞版（内部运行独立事件循环，关闭后才返回）：**

```cpp
sdlgui::MessageBoxResult r = win.showMessageBoxBlocking(
    "确认", "删除文件？",
    sdlgui::MessageBoxType::Question,
    sdlgui::MessageBoxButtons::YesNo);
```

**消息框类型：**

| 类型 | 图标 | 说明 |
|------|------|------|
| `Info` | (i) | 信息提示 |
| `Warning` | (!) | 警告提醒 |
| `Error` | (X) | 错误报告 |
| `Question` | (?) | 确认询问 |

**按钮组合：**

| 组合 | 可用结果 |
|------|---------|
| `OK` | OK |
| `OKCancel` | OK, Cancel |
| `YesNo` | Yes, No |
| `YesNoCancel` | Yes, No, Cancel |
| `RetryCancel` | Retry, Cancel |

| 方法 | 返回 | 说明 |
|------|------|------|
| `showMessageBox(title, msg, type, buttons, callback)` | `void` | 显示非阻塞消息框 |
| `showMessageBoxBlocking(title, msg, type, buttons)` | `MessageBoxResult` | 显示阻塞消息框 |

点击半透明遮罩（对话框外部）返回 `MessageBoxResult::None`。消息框激活时所有标题栏交互被禁用。

#### TextArea（多行文本）

多行可编辑文本（继承 ScrollView）。**默认开启自动换行**；需要时显示垂直滚动条，关闭换行后才启用水平滚动条。支持选择区、拖动选择、Shift+点击、剪贴板和只读模式。

```cpp
auto* ta = root.addChild<sdlgui::TextArea>();
ta->pos(300, 400)->sz(300, 200);
ta->setText("第一行\n第二行\n第三行");
```

| 方法 | 返回 | 说明 |
|------|------|------|
| `setText(string)` | `void` | 设置文本并重置光标 |
| `setTextColor(Color)` | `void` | 设置文字颜色 |
| `setSelectionColor(Color)` | `void` | 设置选择区背景色 |
| `setLineSpacing(int)` | `void` | 行间额外像素（0 = 字体默认行高） |
| `setLetterSpacing(int)` | `void` | 字符间额外像素（0 = 字体默认字距） |
| `setWordWrap(bool)` | `void` | 开启/关闭自动换行（默认 `true`） |
| `setContentInsets(l,t,r,b)` | `void` | 文本/光标/选区的内边距 |
| `setBorderVisible(bool)` / `setBackgroundVisible(bool)` | `void` | 开关边框/背景绘制 |
| `wordWrap()` | `bool` | 当前是否自动换行 |
| `setShowLineNumbers(bool)` | `void` | 显示行号栏 |
| `setAutoIndent(bool)` | `void` | 换行时沿用当前行缩进；行尾为 `{`、`[`、`(` 时增加一级 |
| `setSmartBackspace(bool)` | `void` | 在行首空白中按退格可一次删除一个软 Tab 宽度 |
| `setIndentWithSpaces(bool)` | `void` | 自动缩进使用空格（`true`）或制表符（`false`） |
| `setTabWidth(int)` | `void` | 自动缩进和智能退格使用的 Tab 宽度（1-16） |
| `setReadOnly(bool)` | `void` | 切换只读模式（仍可选择/复制） |
| `text()` | `const string&` | 获取当前文本 |
| `hasSelection()` | `bool` | 是否有选中文本 |
| `selectedText()` | `string` | 获取选中文本（包含换行符） |
| `selectAll()` / `clearSelection()` | `void` | 全选 / 取消选择 |
| `setCursor(line, column)` | `void` | 设置光标（字节列） |
| `cursorLine()` / `cursorColumn()` | `int` | 获取光标行/字节列 |
| `insertText(string)` | `void` | 在光标处插入文本，替换选择区 |
| `insertNewline()` | `void` | 按自动缩进规则插入换行 |
| `backspace()` / `deleteForward()` | `void` | 删除选择区，或删除一个字符 |
| `moveCursorLeft/Right/Up/Down/Home/End(bool select = false)` | `void` | 移动光标；`true` 扩展选择区 |

**键盘控制：** Shift+方向键/Home/End 扩展选择区；Ctrl+A 全选；Ctrl+C/Ctrl+X 复制/剪切；Ctrl+V 粘贴并替换选择区；Enter 插入换行（启用自动缩进时沿用当前缩进）。鼠标拖动或 Shift+点击也可选择。

自动换行会优先在空格、Tab 和常见标点处断行；中文等无空格文本按字符断行。`setWordWrap(false)` 恢复水平滚动。光标位置仍然使用逻辑行和字节列坐标。

行距和字距是 TextArea 专用设置：

```cpp
ta->setLineSpacing(4);   // 每行额外 +4px
ta->setLetterSpacing(1); // 字符间额外 +1px
```

它们会影响渲染、光标/点击定位、选择区、水平滚动和输入法光标位置。

获得焦点时可编辑，`readOnly(true)` 阻止文本修改但仍可选择和复制；光标在获得键盘焦点时闪烁。

#### 键位复写（TextBox / TextArea）

两个文本控件各自持有一个 `TextKeyMap`，默认绑定就是上面列出的快捷键。

```cpp
auto* tb = root.addChild<sdlgui::TextBox>();

// 替换某个动作的键位：
tb->setKeyBinding(sdlgui::TextEditAction::Copy, SDLK_C,
                  SDL_KMOD_CTRL | SDL_KMOD_SHIFT);

// 给同一个动作追加一个键位：
tb->keyMap().add(sdlgui::TextEditAction::Paste, SDLK_INSERT, SDL_KMOD_SHIFT);

// 删除某个动作、清空所有快捷键、恢复默认：
tb->keyMap().remove(sdlgui::TextEditAction::Cut);
tb->keyMap().clear();
tb->resetKeyBindings();
```

可用动作：`CursorLeft/Right/Up/Down/Home/End`、`SelectLeft/Right/Up/Down/Home/End`、`SelectAll`、`Copy`、`Cut`、`Paste`、`Backspace`、`DeleteForward`、`InsertNewline`、`ClearSelection`。TextBox 会忽略上下移动和换行动作。只比较 Shift/Ctrl/Alt/GUI 修饰键，左/右变体（如 `LCTRL` / `RCTRL`）会折叠为通用修饰键；Caps Lock/Num Lock 不影响匹配。`Tab` 不是文本编辑动作；开启 `app.setTabTraversal(true)` 时由焦点遍历消费。

#### RichTextView（富文本视图）

富文本渲染器，由 block + 带样式的 run 组成；它**不解析 Markdown**，由你自己的解析器输出 block/run。

```cpp
auto* rt = root.addChild<sdlgui::RichTextView>();
rt->pos(20, 60)->sz(500, 300);

sdlgui::RichTextRun normal;
normal.text = "Hello ";

sdlgui::RichTextRun bold;
bold.text = "world";
bold.style.color = sdlgui::Color::White;

sdlgui::RichTextStyle codeStyle;
codeStyle.background = sdlgui::Color(60, 60, 70);

sdlgui::RichTextRun code;
code.text = " inline ";
code.style = codeStyle;

sdlgui::RichTextRun link;
link.text = "文档";
link.href = "https://example.com";

rt->addHeading({ normal }, 1);
rt->addParagraph({ normal, bold, code, link });
rt->addListItem({ normal });
rt->addQuote({ normal });
rt->addCodeBlock("int main() {\n  return 0;\n}", codeStyle);

rt->onLinkClick = [](const std::string& href) {
    // 打开链接
};
```

| 方法 | 返回 | 说明 |
|------|------|------|
| `addBlock(block)` | `int` | 添加 block，返回索引 |
| `addParagraph(runs)` / `addHeading(runs, level)` | `int` | 添加段落 / 标题 |
| `addCodeBlock(text, style)` | `int` | 添加不换行代码块 |
| `addListItem(runs, bullet)` / `addQuote(runs)` | `int` | 添加列表项 / 引用 |
| `addTable(table)` | `int` | 添加简单表格 |
| `setBlocks(blocks)` / `clear()` | `void` | 替换 / 清空内容 |
| `blocks()` | `const vector<RichTextBlock>&` | 读取 block |
| `setPadding` / `setLineSpacing` | `void` | 布局间距 |
| `setDefaultTextColor` / `setLinkColor` | `void` | 默认文字 / 链接颜色 |
| `setCodeBackground` / `setCodeBorder` / `setQuoteBarColor` | `void` | 块级样式 |
| `setTableColors(row, header, border)` | `void` | 表格单元格颜色 |
| `setSeparatorColor(Color)` | `void` | 分割线默认颜色 |
| `setActionColors(bg, text)` | `void` | 操作行默认颜色 |
| `setSelectable(bool)` / `selectable()` | `void` / `bool` | 开关选择功能（默认 `true`） |
| `setSelectionColor(Color)` | `void` | 选择区颜色 |
| `selectAll()` / `clearSelection()` | `void` | 全选 / 取消选择 |
| `selectedText()` | `string` | 选中文本（包含 block 间换行） |
| `relayout()` | `void` | 强制重新布局 |
| `onLinkClick` | callback | 点击链接时回调 `href` |
| `onEmptyState` | callback | `blocks()` 为空时绘制空状态 |

`RichTextStyle.font` 是 `std::shared_ptr<Font>`；`nullptr` 使用 `currentFont()`。颜色 alpha 为 0 表示使用视图默认色；`background` 画行内背景，`underline`/`href` 画下划线。

控件有焦点时：`Ctrl+C` / `Cmd+C` 复制选中文本，`Ctrl+A` / `Cmd+A` 全选，`Esc` 清除选区。`setSelectable(false)` 会同时关闭鼠标选择和这些快捷键。

Block 也支持水平布局，适合做消息气泡：

```cpp
sdlgui::RichTextBlock msg;
msg.runs = { yourRun };
msg.align = sdlgui::RichTextAlign::Right;
msg.maxWidth = 320;       // 总宽度上限，包含 paddingX
msg.paddingX = 12;        // 背景块左右内边距
msg.background = sdlgui::Color(42, 48, 60);
rt->addBlock(msg);
```

- `align` 支持 `Left` / `Center` / `Right`。
- `maxWidth == 0`：左对齐时占满可用宽度；居中/右对齐时按内容宽度收缩。
- `maxWidth > 0`：按内容宽度收缩，并限制在 `maxWidth` 以内。
- `paddingX` 增加背景块内部左右留白，同时减少换行可用宽度。
- 换行使用扣除 padding 后的内容宽度，窗口缩放时会自动重新布局。
- 表格目前仍使用整行宽度布局。

**Separator（分割线）**

```cpp
sdlgui::RichTextBlock line;
line.type = sdlgui::RichBlockType::Separator;
line.separatorThickness = 1;
line.separatorColor = sdlgui::Color(80, 80, 86);
line.separatorMarginTop = 6;
line.separatorMarginBottom = 6;
rt->addBlock(line);
```

分割线横跨当前 block 可用宽度，参与正常纵向排版和滚动；没有文本，不参与选择。

**Card（卡片）**

```cpp
sdlgui::RichTextBlock card;
card.isCard = true;
card.background = sdlgui::Color(30, 32, 40);
card.borderWidth = 1;
card.borderColor = sdlgui::Color(70, 72, 82);
card.paddingX = 12;
card.paddingY = 10;

card.children.push_back(titleBlock);
card.children.push_back(codeBlock);
card.children.push_back(separatorBlock);
card.children.push_back(resultBlock);
rt->addBlock(card);
```

子块在同一个背景和边框内纵向排列，支持嵌套 Card。Card 本身不产生文本；子块按正常规则参与选择。

**Actions（操作区）**

```cpp
sdlgui::RichTextAction allow;
allow.text = "允许";
allow.textAlign = sdlgui::RichTextAlign::Left;
allow.paddingX = 12;
allow.onClick = [] { /* ... */ };

sdlgui::RichTextAction deny;
deny.text = "拒绝";
deny.enabled = false;

sdlgui::RichTextBlock actions;
actions.type = sdlgui::RichBlockType::Actions;
actions.actions = { allow, deny };
actions.actionsVertical = true;
actions.actionHeight = 32;
actions.actionGap = 8;
rt->addBlock(actions);
```

操作区在内容流中绘制，随视图滚动，并由 `RichTextView` 负责点击命中。按钮文字不进入可选中文本，因此复制/选择不会包含按钮标签。它们是内容流里的操作行，不是可聚焦子控件；需要键盘焦点时请在 `RichTextView` 外使用普通 `Button`。

表格使用 `RichTable`、`RichTableRow` 和 `RichTableCell`：

```cpp
sdlgui::RichTable table;
table.headerRow = true;
table.cellPadding = 6;
table.columnWeights = { 1.0f, 2.0f }; // 相对列宽

sdlgui::RichTableRow header;
header.cells = { { { "名称", {}, "" } }, { { "说明", {}, "" } } };
table.rows.push_back(header);

sdlgui::RichTableRow row;
row.cells = { { { "alpha", {}, "" } }, { { "单元格内会自动换行的长文本", {}, "" } } };
table.rows.push_back(row);

rt->addTable(table);
```

单元格文本会在列宽内自动换行，支持选择、样式 run 和链接。`columnWeights` 是相对列宽；为空时等宽。暂不支持行列合并。

选择：拖动选择、Shift+点击扩展、Ctrl+A / Ctrl+C / Esc（控件有焦点时）。调用 `setSelectable(false)` 可关闭。

当前支持：段落、标题、代码块、引用、列表项、表格、混合字体/颜色、自动换行、链接、选择复制。行内图片、可折叠 block 暂未实现。

#### Image（图片）

显示图片。需定义 `#define SDLGUI_USE_SDL_IMAGE` 并安装 SDL3_image。

```cpp
#define SDLGUI_USE_SDL_IMAGE
#include "sdlgui.hpp"

auto* img = root.addChild<sdlgui::Image>();
img->pos(400, 40)->sz(200, 150);
img->load("photo.png");
```

| 方法 | 返回 | 说明 |
|------|------|------|
| `load(path)` | `bool` | 加载图片文件 |
| `setTexture(SDL_Texture*)` | `void` | 使用现有纹理（转移所有权） |
| `setColor(Color)` | `void` | 设置调色 |

**注意：** 不定义 `SDLGUI_USE_SDL_IMAGE` 时使用 `Image` 会导致编译错误。

### 字体系统

#### 内置位图字体

在包含 `sdlgui.hpp` 前定义 `SDLGUI_USE_BUILTIN_FONT` 以使用内置位图字体。字体数据由 `fontData.hpp` 提供。

字形数据来自 **GNU Unifont**（https://unifoundry.com/unifont/），并转换成本库使用的 8×16 / 16×16 原始位图表。

```cpp
#define SDLGUI_USE_BUILTIN_FONT
#include "sdlgui.hpp"
```

内置字体使用**双查找表**覆盖 Unicode BMP（码点 0x0000-0xFFFC）：

| 数组 | 维度 | 说明 |
|------|------|------|
| `font8x16` | `[256][16]` | 8×16 字形，码点 0x0000-0x00FF（Latin-1） |
| `font16x16` | `[65533][32]` | 16×16 字形，码点 0x0000-0xFFFC（全 BMP） |

**查找逻辑：**

1. 若 `cp < 256` 且 `font8x16[cp]` 有非零内容 → 使用 8×16 字形，宽度 = 8
2. 否则若 `cp < 65533` → 使用 16×16 字形，宽度 = 16
3. 否则 → 跳过，宽度 = 8

**字体数据格式：**

- 8×16：每字形 = 16 行 × 1 字节（每行 8 像素，bit7 为最左像素）
- 16×16：每字形 = 16 行 × 2 字节（左字节 + 右字节）

**全角 vs 半角：** U+0020（半角空格）→ 8px。U+3000（全角空格，CJK）→ 16px。查找逻辑自动处理。

**你可以用完整 BMP 数据替换 `fontData.hpp`。** 仓库自带的文件已经包含 `font8x16` 和完整的 `font16x16`（全 BMP），按相同格式替换即可。

#### 运行时切换字体

运行时可以替换当前字体：

```cpp
// 切换到自定义/加载的字体
sdlgui::setFont(myFont);

// 切回内置 Unifont（或 fallback 字体）
sdlgui::setFont(nullptr);

sdlgui::Font* f = sdlgui::currentFont();
uint64_t epoch = sdlgui::fontEpoch();
```

`Font` 提供库需要的测量和绘制接口：

```cpp
class Font
{
public:
    virtual int  lineHeight() const;
    virtual int  ascent() const;
    virtual int  measure(const char* text) const = 0;
    virtual int  measureTo(const char* text, int bytePos) const = 0;
    virtual int  advance(uint32_t codepoint) const = 0;
    virtual void draw(SDL_Renderer*, const char* text,
                      int x, int y, Color color) = 0;
    //可选：根据 Application::uiScale() 重新光栅化字形。
    virtual void setRasterScale(float scale) {}
};
```

自定义 TTF/Canvas 字体应重写 `setRasterScale(scale)`：按 `fontSize * scale` 光栅化，但 metrics 和绘制坐标仍使用逻辑尺寸。`Application::setUiScale()` 和 `setFont()` 会自动调用。

不想继承时可以用回调适配器：

```cpp
auto font = std::make_shared<sdlgui::CallbackFont>(
    [](const char* s) { return measureText(s); },
    [](const char* s, int n) { return measurePrefix(s, n); },
    [](uint32_t cp) { return advance(cp); },
    [](SDL_Renderer* r, const char* s, int x, int y, sdlgui::Color c) {
        drawText(r, s, x, y, c);
    },
    16, 16); // 行高、ascent

sdlgui::setFont(font);
```

切换字体会：

- 清空文本纹理缓存；
- 增加 `fontEpoch()`，使 `Label` 布局缓存失效；
- 重新测量 `setAutoHeight(true)` / `setAutoWidth(true)` 的控件，并重排其父容器 / Box；
- 刷新 TextBox/TextArea 的滚动状态并标记控件重绘；
- 请求窗口重绘。

如果文字控件仍使用按旧字体写死的高度，换成更大的字体后就会被裁剪。需要高度跟随文本时请开启自动尺寸：

```cpp
label->setAutoHeight(true);
vbox->setAutoHeight(true);  // 子项 autoH 时 Box 也会随之长高
```

`CallbackFont` 未提供测量回调时按每字符 8px 兜底。`setFontRenderer()` 仅作为未编译内置字体时的旧接口保留，优先使用 `setFont()`。

**注意：** 控件尺寸默认仍是显式设置的；只有开启 `setAutoHeight()` / `setAutoWidth()` 后才会随内容变化。需要高度跟随字体时请开启自动高度，否则更大的字体可能被固定矩形裁掉。

#### 兜底行为

无内置字体且无自定义 `Font` 时，`drawText()` 为空操作——不渲染文本，文本宽度默认每个字符 8px。

### 主题

一键应用整窗样式：

```cpp
win.setTheme(sdlgui::Theme::light());   // 白色主题
win.setTheme(sdlgui::Theme::dark());    // 默认深色主题

// 所有窗口：
app.setTheme(sdlgui::Theme::light());

// 只应用到一个控件子树：
sdlgui::applyTheme(root, sdlgui::Theme::light());
```

`Theme` 包含窗口、按钮、输入框、面板、列表、滚动条、菜单、弹出层、TextArea、Slider、标题栏、消息框、CheckBox 和 SplitView 分隔条的颜色。应用主题后仍可单独覆盖某个控件的颜色。

**各控件颜色 API：**

| 控件 | Setter |
|------|--------|
| `Window` | `setBackgroundColor`、`setTitleBarColor`、`setTitleTextColor`、`setTitleButtonColors`、`setTitleCloseColor` |
| `Label` | `setTextColor` |
| `Button` | `setTextColor`、`setBgColor`、`setBorderColor` |
| `CheckBox` | `setTextColor`、`setCheckColor`、`setBoxColor`、`setBorderColor` |
| `TextBox` | `setBackgroundColor`、`setTextColor`、`setPlaceholderColor`、`setCursorColor`、`setSelectionColor`、`setBorderColor` |
| `TextArea` | `setBackgroundColor`、`setTextColor`、`setCursorColor`、`setSelectionColor`、`setBorderColor` |
| `ListBox` | `setBackgroundColor`、`setTextColor`、`setSelectionColor`、`setHoverColor`、`setBorderColor` |
| `ScrollView` | `setScrollbarColors(track, thumb)` |
| `Slider` | `setTrackColor`、`setFillColor`、`setThumbColor`、`setBorderColor` |
| `MenuBar` | `setBackgroundColor`、`setTextColor`、`setHoverColor`、`setPopupColor`、`setBorderColor`、`setItemHoverColor`、`setDisabledTextColor`、`setShortcutColor`、`setAccentColor` |
| `ContextMenu` | `setBackgroundColor`、`setTextColor`、`setHoverColor`、`setBorderColor`、`setDisabledTextColor`、`setSeparatorColor` |
| `DropDown` | `setBackgroundColor`、`setTextColor`、`setBorderColor`、`setArrowColor`、`setPopupColor`、`setSelectionColor` |
| `ComboBox` | 同 DropDown，另加 `setButtonColor` |
| `MessageBox` | `setOverlayColor`、`setBackgroundColor`、`setBorderColor`、`setTextColor`、`setTitleColors`、`setButtonColors`、`applyTheme` |
| `SplitView` | `setDividerColor` |

### 颜色参考

```cpp
struct Color {
    uint8_t r, g, b, a;
    
    static const Color Black;       // (0,0,0)
    static const Color White;       // (255,255,255)
    static const Color Red;         // (255,0,0)
    static const Color Green;       // (0,255,0)
    static const Color Blue;        // (0,0,255)
    static const Color Gray;        // (128,128,128)
    static const Color LightGray;   // (192,192,192)
    static const Color Transparent; // (0,0,0,0)
};
```

颜色也可内联构造：`sdlgui::Color(120, 30, 50)` 或使用初始化列表：`{120, 30, 50}`。

### 事件结构

```cpp
struct MouseEvent {
    int x, y;             // 相对于目标控件的坐标
    int screenX, screenY; // 屏幕绝对坐标
    uint8_t button;       // SDL_BUTTON_LEFT, SDL_BUTTON_RIGHT 等
    uint8_t clicks;       // 单击为 1，双击为 2
};

struct KeyEvent {
    SDL_Keycode key;      // SDLK_* 常量
    uint16_t mod;          // KMOD_* 修饰键掩码
    char text;             // 可打印字符（控制键为 0）
};
```

### 编码规范

| 类别 | 规范 | 示例 |
|------|------|------|
| 类/结构体 | 大驼峰 | `class PushButton` |
| 函数/变量/成员 | 小驼峰 | `setEnabled()`、`isVisible_` |
| 缩进 | Allman | 大括号独占一行 |
| 注释 | `//[TEXT]` `//` 后无空格 | `//this is correct` |
| 文件名 | 小驼峰 | `sdlgui.hpp`、`fontData.hpp` |

### 编译

#### 使用 Make（推荐）

```bash
make        # 发布版
make debug  # 调试版
make clean  # 清理
```

Makefile 使用 `pkg-config` 查找 SDL3。如果 `pkg-config` 不可用，可设置 `SDL3_CFLAGS` 和 `SDL3_LIBS` 环境变量。

#### 手动编译

```bash
# Linux / macOS
g++ -std=c++14 $(pkg-config --cflags sdl3) -o demo demo.cc $(pkg-config --libs sdl3)

# Windows（MSYS2/MinGW）
g++ -std=c++14 $(pkg-config --cflags sdl3) -o demo demo.cc $(pkg-config --libs sdl3)
```

#### 无 pkg-config

```bash
g++ -std=c++14 -I/path/to/SDL3/include -L/path/to/SDL3/lib -lSDL3 -mwindows -o demo demo.cc
```

### 示例

#### 最小应用

```cpp
#define SDLGUI_IMPLEMENTATION
#define SDLGUI_USE_BUILTIN_FONT
#include "sdlgui.hpp"

int main()
{
    sdlgui::Application app;
    app.addWindow("最小示例", 400, 300);
    app.run();
    return 0;
}
```

#### 自定义颜色按钮

```cpp
auto* btn = root.addChild<sdlgui::Button>();
btn->pos(20, 80)->sz(120, 30);
btn->setText("红色按钮");
btn->setBgColor(
    sdlgui::Color(120, 20, 20),   // 正常
    sdlgui::Color(180, 30, 30),   // 悬停
    sdlgui::Color(80, 10, 10)     // 按下
);
btn->onClick = [](sdlgui::MouseEvent&) {
    printf("红色按钮被点击！\n");
};
```

#### 复选框切换

```cpp
auto* cb = root.addChild<sdlgui::CheckBox>();
cb->pos(20, 130)->sz(160, 24);
cb->setText("启用功能");
cb->setCheckColor(sdlgui::Color(0, 200, 0));
// 点击、Space 或 Enter 会自动切换
```

#### 文本框样式

```cpp
auto* tb = root.addChild<sdlgui::TextBox>();
tb->pos(20, 180)->sz(200, 28);
tb->setPlaceholder("请输入昵称");
tb->setMaxLength(16);
tb->setBackgroundColor(sdlgui::Color(240, 240, 240));
tb->setTextColor(sdlgui::Color(30, 30, 30));
tb->setCursorColor(sdlgui::Color::Red);

auto* pw = root.addChild<sdlgui::TextBox>();
pw->pos(20, 216)->sz(200, 28);
pw->setPassword(true);
pw->setPlaceholder("密码");
```

#### 画布自定义绘制

```cpp
auto* cv = root.addChild<sdlgui::Canvas>();
cv->pos(20, 240)->sz(300, 200);
cv->onDraw = [](SDL_Renderer* r, const sdlgui::Rect& a)
{
    // 网格
    SDL_SetRenderDrawColor(r, 60, 60, 60, 255);
    for (int x = 0; x < a.w; x += 40)
        SDL_RenderLine(r, a.x + x, a.y, a.x + x, a.y + a.h);
    for (int y = 0; y < a.h; y += 40)
        SDL_RenderLine(r, a.x, a.y + y, a.x + a.w, a.y + y);

    // 对角线
    SDL_SetRenderDrawColor(r, 200, 100, 50, 255);
    SDL_RenderLine(r, a.x, a.y, a.x + a.w, a.y + a.h);
};
```

### 已知限制

- 多窗口的焦点/事件路由已实现，但多窗口场景仍未充分测试
- TextBox 和 TextArea 已有选择区；ComboBox 还没有选择模型
- TextArea 还没有撤销/重做、语法高亮和大文件缓冲优化；Tab 焦点遍历默认关闭
- 无网格布局、停靠或 CSS 式 flex；布局原语为锚点、HBox/VBox 和 SplitView
- 无全屏 API，只支持窗口模式
- 内置字体是固定 16px 位图；运行时 `Font` 可以改变 metrics/绘制，逻辑分辨率会处理 HiDPI 坐标，控件尺寸默认显式设置（可用 `setAutoHeight`/`setAutoWidth` 跟随内容），没有字距/连字处理
- `Label` 按字符换行；`TextArea` 使用单词级换行
- `RichTextView` 还没有行内图片、可折叠 block，以及表格行列合并
- 旧接口 `setFontRenderer()` 按每字符 8px 测量；需要真实 metrics 请用 `setFont()`
- 按需重绘是整窗级的，没有局部脏矩形
- TextBox / TextArea 会绘制输入法内联预编辑文本；同时把光标矩形传给 `SDL_SetTextInputArea`，让候选窗跟随光标
- 无无障碍支持
- `SDLGUI_IMPLEMENTATION` 只能在一个编译单元中定义

### 许可

BSD 2-Clause License，见 [LICENSE](LICENSE)。
