#ifndef SDLGUI_HPP
#define SDLGUI_HPP

#include <SDL3/SDL.h>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <cstdint>
#include <cstring>
#include <cmath>
#include <cctype>
#include <cwctype>
#include <algorithm>

#ifdef SDLGUI_USE_SDL_IMAGE
#include <SDL3_image/SDL_image.h>
#endif

//==============================================================================
// Forward declarations
//==============================================================================
namespace sdlgui {

struct Rect;
struct Color;
struct MouseEvent;
struct KeyEvent;
class Widget;
class Canvas;
class Label;
class Canvas;
class Label;
class Button;
class CheckBox;
class TextBox;
class ListBox;
class Slider;
class DropDown;
class ComboBox;
class TextArea;
class ScrollView;
class MenuBar;
struct ContextMenuItem;
class ContextMenu;
class Window;
class MessageBox;
class Application;

enum class MessageBoxType   { Info, Warning, Error, Question };
enum class MessageBoxButtons { OK, OKCancel, YesNo, YesNoCancel, RetryCancel };
enum class MessageBoxResult  { None, OK, Cancel, Yes, No, Retry };

//==============================================================================
// Rect
//==============================================================================
struct Rect
{
    int x, y, w, h;
};

//==============================================================================
// Color
//==============================================================================
struct Color
{
    uint8_t r, g, b, a;

    Color() : r(0), g(0), b(0), a(255) {}
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
        : r(r), g(g), b(b), a(a) {}

    uint32_t toUint32() const
    {
        return (uint32_t(r) << 24) | (uint32_t(g) << 16)
             | (uint32_t(b) << 8)  | uint32_t(a);
    }

    static const Color Black;
    static const Color White;
    static const Color Red;
    static const Color Green;
    static const Color Blue;
    static const Color Gray;
    static const Color LightGray;
    static const Color Transparent;
};

//==============================================================================
// Theme
//==============================================================================
struct Theme
{
    Color windowBg      = Color(30, 30, 30);
    Color text          = Color::White;
    Color disabledText  = Color(100, 100, 100);
    Color mutedText     = Color(160, 160, 160);
    Color border        = Color(100, 100, 100);

    Color buttonBg      = Color(60, 60, 60);
    Color buttonHover   = Color(80, 80, 80);
    Color buttonPressed = Color(40, 40, 40);
    Color buttonText    = Color::White;
    Color buttonBorder  = Color(100, 100, 100);

    Color inputBg       = Color::White;
    Color inputText     = Color::Black;
    Color inputPlaceholder = Color(130, 130, 130);
    Color inputBorder   = Color(140, 140, 140);
    Color inputCursor   = Color::Black;
    Color inputSelection = Color(50, 100, 180);

    Color panelBg       = Color(40, 40, 40);
    Color panelBorder   = Color(70, 70, 70);
    Color rowHover      = Color(50, 50, 60);
    Color rowSelected   = Color(40, 80, 140);
    Color rowSelectedText = Color::White;

    Color scrollTrack   = Color(40, 40, 40);
    Color scrollThumb   = Color(100, 100, 100);

    Color menuBarBg     = Color(50, 50, 50);
    Color menuHover     = Color(65, 65, 65);
    Color menuText      = Color::White;
    Color menuDisabled  = Color(100, 100, 100);
    Color menuShortcut  = Color(160, 160, 160);
    Color menuPopupBg   = Color(40, 40, 40);
    Color menuItemHover = Color(50, 100, 180);
    Color menuAccent    = Color(60, 120, 200);

    Color textAreaBg    = Color(25, 25, 28);
    Color textAreaText  = Color::White;
    Color textAreaCursor = Color(200, 200, 200);
    Color textAreaSelection = Color(50, 100, 180, 180);

    Color sliderTrack   = Color(60, 60, 60);
    Color sliderFill    = Color(80, 140, 200);
    Color sliderThumb   = Color(140, 140, 140);
    Color sliderThumbHover = Color(180, 180, 180);
    Color sliderBorder  = Color(100, 100, 100);

    Color titleBarBg    = Color(50, 50, 50);
    Color titleText     = Color::White;
    Color titleButton   = Color(50, 50, 50);
    Color titleButtonHover = Color(80, 80, 80);
    Color titleCloseHover = Color(180, 30, 30);

    Color overlay       = Color(0, 0, 0, 180);
    Color dialogBg      = Color(45, 45, 45);
    Color dialogBorder  = Color(70, 70, 70);
    Color dialogTitleBg = Color(55, 55, 55);
    Color dialogTitleText = Color::White;
    Color dialogText    = Color::White;
    Color dialogButton  = Color(60, 60, 60);
    Color dialogButtonHover = Color(85, 85, 85);
    Color dialogButtonBorder = Color(100, 100, 100);
    Color dialogButtonText = Color::White;

    Color checkboxBox   = Color(255, 255, 255);
    Color checkboxBorder = Color(100, 100, 100);
    Color checkboxCheck = Color(0, 160, 0);

    Color divider       = Color(70, 70, 75);

    static Theme dark() { return Theme{}; }
    static Theme light();
};

void applyTheme(class Widget& root, const Theme& theme);
void applyTheme(class Window& window, const Theme& theme);
void applyTheme(class Application& app, const Theme& theme);

//==============================================================================
// Layout anchors
//==============================================================================
enum AnchorFlag : uint8_t
{
    AnchorLeft   = 1 << 0,
    AnchorRight  = 1 << 1,
    AnchorTop    = 1 << 2,
    AnchorBottom = 1 << 3
};

//==============================================================================
// MouseEvent
//==============================================================================
struct MouseEvent
{
    int    x = 0, y = 0;
    int    screenX = 0, screenY = 0;
    uint8_t button = 0;
    uint8_t clicks = 0;
};

//==============================================================================
// KeyEvent
//==============================================================================
struct KeyEvent
{
    SDL_Keycode key  = 0;
    uint16_t    mod  = 0;
    char        text = 0;
};

//==============================================================================
// Text editing key bindings
//==============================================================================
enum class TextEditAction
{
    None,
    CursorLeft, CursorRight, CursorUp, CursorDown, CursorHome, CursorEnd,
    SelectLeft, SelectRight, SelectUp, SelectDown, SelectHome, SelectEnd,
    SelectAll, Copy, Cut, Paste,
    Backspace, DeleteForward, InsertNewline, ClearSelection
};

class TextKeyMap
{
public:
    TextKeyMap();

    void reset();
    void clear();
    void set(TextEditAction action, SDL_Keycode key, SDL_Keymod mod = SDL_KMOD_NONE);
    void add(TextEditAction action, SDL_Keycode key, SDL_Keymod mod = SDL_KMOD_NONE);
    void remove(TextEditAction action);
    TextEditAction find(SDL_Keycode key, SDL_Keymod mod) const;

private:
    struct Binding
    {
        TextEditAction action;
        SDL_Keycode    key;
        uint16_t       mod;
    };

    std::vector<Binding> bindings_;
    static uint16_t normalize(SDL_Keymod mod);
};

//==============================================================================
// Font
//==============================================================================
class Font
{
public:
    virtual ~Font() {}

    virtual uint64_t id() const
    {
        return (uint64_t)(uintptr_t)this;
    }

    virtual int lineHeight() const { return 16; }
    virtual int ascent() const { return 16; }

    virtual int measure(const char* text) const = 0;
    virtual int measureTo(const char* text, int bytePos) const = 0;
    virtual int advance(uint32_t codepoint) const = 0;

    virtual void draw(SDL_Renderer* renderer, const char* text,
                      int x, int y, Color color) = 0;
};

using FontMeasure   = std::function<int(const char*)>;
using FontMeasureTo = std::function<int(const char*, int)>;
using FontAdvance   = std::function<int(uint32_t)>;
using FontDraw      = std::function<void(SDL_Renderer*, const char*,
                                         int, int, Color)>;

class CallbackFont : public Font
{
public:
    CallbackFont(FontMeasure measure = {},
                 FontMeasureTo measureTo = {},
                 FontAdvance advance = {},
                 FontDraw draw = {},
                 int lineHeight = 16,
                 int ascent = 16);

    int lineHeight() const override { return lineHeight_; }
    int ascent() const override { return ascent_; }

    int measure(const char* text) const override;
    int measureTo(const char* text, int bytePos) const override;
    int advance(uint32_t codepoint) const override;
    void draw(SDL_Renderer* renderer, const char* text,
              int x, int y, Color color) override;

private:
    FontMeasure   measure_;
    FontMeasureTo measureTo_;
    FontAdvance   advance_;
    FontDraw      draw_;
    int           lineHeight_ = 16;
    int           ascent_ = 16;
};

void setFont(std::shared_ptr<Font> font);
Font* currentFont();
std::shared_ptr<Font> defaultFont();
uint64_t fontEpoch();

//==============================================================================
// Legacy font renderer type
//==============================================================================
using FontRenderer = void (*)(SDL_Renderer*, const char* text,
                              int x, int y, Color color);

void setFontRenderer(FontRenderer fn);

//==============================================================================
// Widget
//==============================================================================
class Widget
{
public:
    explicit Widget(Widget* parent = nullptr);
    virtual ~Widget();

    //position
    void      setPosition(int x, int y);
    void      setSize(int w, int h);
    int       x()       const { return x_; }
    int       y()       const { return y_; }
    int       width()   const { return width_; }
    int       height()  const { return height_; }

    int       absoluteX() const;
    int       absoluteY() const;
    Rect      absoluteRect() const;

    //tree
    void      addChild(std::unique_ptr<Widget> child);
    void      removeChild(Widget* child);

    template<typename T, typename... Args>
    T* addChild(Args&&... args)
    {
        auto c = std::make_unique<T>(this, std::forward<Args>(args)...);
        auto* r = c.get();
        addChild(std::move(c));
        return r;
    }

    Widget*   parent() const { return parent_; }
    const std::vector<std::unique_ptr<Widget>>& children() const { return children_; }

    //visibility
    void      setVisible(bool v);
    bool      visible() const { return visible_; }

    //enabled
    void      setEnabled(bool e);
    bool      enabled() const { return enabled_; }

    //keyboard focus participation
    void      setFocusable(bool f) { focusable_ = f; }
    bool      focusable() const { return focusable_; }

    //layout
    void      setAnchors(int flags);
    void      setAnchors(int flags, int left, int top, int right, int bottom);
    void      setAnchorMargins(int left, int top, int right, int bottom);
    int       anchors() const { return anchorFlags_; }
    void      setStretch(float weight);
    float     stretchWeight() const { return layoutStretch_; }
    void      setFillCross(bool on);
    bool      fillCross() const { return fillCross_; }
    void      relayout();

    //chain aliases
    Widget* pos(int x, int y) { setPosition(x, y); return this; }
    Widget* sz(int w, int h)  { setSize(w, h); return this; }
    Widget* vis(bool v)       { setVisible(v); return this; }
    Widget* ena(bool v)       { setEnabled(v); return this; }
    Widget* anch(int flags)   { setAnchors(flags); return this; }
    Widget* stretch(float w)  { setStretch(w); return this; }
    Widget* fillCross(bool on = true) { setFillCross(on); return this; }

    //window access
    class Window* getWindow() const { return window_; }

    //events
    std::function<void(MouseEvent&)> onMouseDown;
    std::function<void(MouseEvent&)> onMouseUp;
    std::function<void(MouseEvent&)> onMouseMove;
    std::function<void(MouseEvent&)> onClick;
    std::function<void(MouseEvent&)> onMouseEnter;
    std::function<void(MouseEvent&)> onMouseLeave;
    std::function<void(KeyEvent&)>   onKeyDown;
    std::function<void(KeyEvent&)>   onKeyUp;
    std::function<void()>            onFocus;
    std::function<void()>            onBlur;
    std::function<void(MouseEvent&, ContextMenu&)> onContextMenu;

    //render
    virtual void onRender(SDL_Renderer* renderer);

    //hit test
    virtual bool hitTest(int absX, int absY) const;

    //IME caret rectangle in window coordinates (false for non-text widgets)
    virtual bool textInputRect(Rect& rect) const { (void)rect; return false; }

    //scroll
    virtual void onMouseWheel(int delta);

    //popups (DropDown / ComboBox)
    virtual bool isPopupOpen() const { return false; }
    virtual void closePopup() {}

    //dirty
    void  markDirty();
    bool  dirty() const { return dirty_; }
    void  clearDirty()  { dirty_ = false; }

    //redraw
    virtual bool wantsContinuousRedraw() const { return false; }
    void requestRedraw();

protected:
    int    x_ = 0, y_ = 0, width_ = 100, height_ = 50;
    int    renderOffX_ = 0, renderOffY_ = 0;
    bool   visible_ = true;
    bool   enabled_ = true;
    bool   focusable_ = false;
    bool   dirty_ = true;

    uint8_t anchorFlags_ = 0;
    int     anchorL_ = 0, anchorT_ = 0, anchorR_ = 0, anchorB_ = 0;
    float   layoutStretch_ = 0.0f;
    bool    fillCross_ = false;
    bool    layingOut_ = false;

    Widget* parent_ = nullptr;
    class Window* window_ = nullptr;
    std::vector<std::unique_ptr<Widget>> children_;

    void setWindowRecursive(class Window* w);
    void setRenderOffset(int dx, int dy) { renderOffX_ = dx; renderOffY_ = dy; }
    void setBoundsInternal(int x, int y, int w, int h);
    virtual void layoutChildren();
    virtual void onFontChanged() { markDirty(); }
    void notifyFontChanged();

    friend class Window;
    friend class Application;
    friend class ScrollView;
    friend class TextArea;
    friend class Box;
    friend class SplitView;
};

//==============================================================================
// Box — horizontal / vertical linear layout
//==============================================================================
class Box : public Widget
{
public:
    enum class Orientation { Horizontal, Vertical };

    explicit Box(Widget* parent = nullptr,
                 Orientation orientation = Orientation::Horizontal);

    void setOrientation(Orientation o);
    Orientation orientation() const { return orientation_; }

    void setPadding(int padding);
    int  padding() const { return padding_; }

    void setSpacing(int spacing);
    int  spacing() const { return spacing_; }

protected:
    void layoutChildren() override;

private:
    Orientation orientation_ = Orientation::Horizontal;
    int padding_ = 0;
    int spacing_ = 0;
};

class HBox : public Box
{
public:
    explicit HBox(Widget* parent = nullptr)
        : Box(parent, Orientation::Horizontal) {}
};

class VBox : public Box
{
public:
    explicit VBox(Widget* parent = nullptr)
        : Box(parent, Orientation::Vertical) {}
};

//==============================================================================
// SplitView — two panes with a draggable divider
//==============================================================================
class SplitView : public Widget
{
public:
    enum class Orientation { SideBySide, Stacked };

    explicit SplitView(Widget* parent = nullptr);

    void setOrientation(Orientation o);
    Orientation orientation() const { return orientation_; }

    void setSplit(float ratio);
    float split() const { return ratio_; }

    void setMinPaneSize(int px);
    int  minPaneSize() const { return minPane_; }

    void setDividerWidth(int px);
    int  dividerWidth() const { return dividerW_; }

    void setDividerColor(Color color);
    Color dividerColor() const { return dividerColor_; }

protected:
    void layoutChildren() override;
    void onRender(SDL_Renderer* renderer) override;

private:
    int dividerPos() const;
    void splitFromMouse(int localX, int localY);

    Orientation orientation_ = Orientation::SideBySide;
    float ratio_ = 0.5f;
    int   minPane_ = 40;
    int   dividerW_ = 6;
    Color dividerColor_ = Color(70, 70, 75);
    bool  dragging_ = false;
};

//==============================================================================
// Canvas — user custom drawing
//==============================================================================
class Canvas : public Widget
{
public:
    using Widget::Widget;

    std::function<void(SDL_Renderer*, const Rect&)> onDraw;

    //Continuous redraw is on by default when onDraw is set, so existing
    //animated canvases keep working. Turn it off for static content.
    void setContinuousRedraw(bool enabled) { continuousRedraw_ = enabled; markDirty(); }
    bool continuousRedraw() const { return continuousRedraw_; }
    bool wantsContinuousRedraw() const override
    {
        return continuousRedraw_ && (bool)onDraw;
    }

protected:
    void onRender(SDL_Renderer* renderer) override;

private:
    bool continuousRedraw_ = true;
};

//==============================================================================
// Image — requires #define SDLGUI_USE_SDL_IMAGE
//==============================================================================
#ifdef SDLGUI_USE_SDL_IMAGE
class Image : public Widget
{
public:
    using Widget::Widget;

    bool load(const char* path);
    void setTexture(SDL_Texture* tex);
    void setColor(Color color);
    ~Image();

protected:
    void onRender(SDL_Renderer* renderer) override;

private:
    SDL_Texture* texture_ = nullptr;
    SDL_Surface* surfaceCache_ = nullptr;
    int texW_ = 0, texH_ = 0;
    Color modColor_ = Color::White;
};
#endif //SDLGUI_USE_SDL_IMAGE

//==============================================================================
// Label
//==============================================================================
class Label : public Widget
{
public:
    using Widget::Widget;

    void     setText(const std::string& text);
    void     setTextColor(Color color);
    void     setWordWrap(bool wrap);
    const std::string& text()      const { return text_; }
    Color    textColor()           const { return textColor_; }
    bool     wordWrap()            const { return wordWrap_; }

    Widget* txt(const std::string& s) { setText(s); return this; }
    Widget* clr(Color c)              { setTextColor(c); return this; }

protected:
    void onRender(SDL_Renderer* renderer) override;
    void onFontChanged() override;

private:
    std::string text_;
    Color       textColor_ = Color::White;
    bool        wordWrap_  = false;
    std::vector<std::string> layout_;
    int         layoutWidth_ = -1;
    bool        layoutWrap_  = false;
    uint64_t    layoutFontEpoch_ = 0;
};

//==============================================================================
// Button
//==============================================================================
enum class ButtonState { Normal, Hover, Pressed };

class Button : public Widget
{
public:
    explicit Button(Widget* parent = nullptr);

    void     setText(const std::string& text);
    void     setTextColor(Color color);
    void     setBgColor(Color normal, Color hover, Color pressed);
    void     setBorderColor(Color color);
    const std::string& text() const { return text_; }

    Widget* txt(const std::string& s) { setText(s); return this; }
    Widget* clr(Color c)              { setTextColor(c); return this; }
    Widget* bg(Color n, Color h, Color p) { setBgColor(n, h, p); return this; }
    Widget* borderClr(Color c)        { setBorderColor(c); return this; }

protected:
    void onRender(SDL_Renderer* renderer) override;

private:
    std::string text_;
    ButtonState state_ = ButtonState::Normal;
    Color       textColor_ = Color::White;
    Color       bgNormal_  = Color(60, 60, 60);
    Color       bgHover_   = Color(80, 80, 80);
    Color       bgPressed_ = Color(40, 40, 40);
    Color       borderColor_ = Color(100, 100, 100);
};

//==============================================================================
// CheckBox
//==============================================================================
class CheckBox : public Widget
{
public:
    explicit CheckBox(Widget* parent = nullptr);

    void     setText(const std::string& text);
    void     setChecked(bool checked);
    void     setTextColor(Color color);
    void     setCheckColor(Color color);
    void     setBoxColor(Color color);
    void     setBorderColor(Color color);
    bool     isChecked() const { return checked_; }
    const std::string& text() const { return text_; }

    Widget* txt(const std::string& s)   { setText(s); return this; }
    Widget* clr(Color c)                { setTextColor(c); return this; }
    Widget* checkClr(Color c)           { setCheckColor(c); return this; }
    Widget* boxClr(Color c)             { setBoxColor(c); return this; }

protected:
    void onRender(SDL_Renderer* renderer) override;

private:
    std::string text_;
    bool        checked_ = false;
    Color       textColor_ = Color::White;
    Color       checkColor_ = Color(0, 160, 0);
    Color       boxColor_ = Color(255, 255, 255);
    Color       borderColor_ = Color(100, 100, 100);
};

//==============================================================================
// TextBox — single line text input
//==============================================================================
class TextBox : public Widget
{
public:
    explicit TextBox(Widget* parent = nullptr);

    void     setText(const std::string& text);
    void     setTextColor(Color color);
    void     setBackgroundColor(Color color);
    void     setCursorColor(Color color);
    void     setSelectionColor(Color color);
    void     setBorderColor(Color color);
    void     setPlaceholder(const std::string& text);
    void     setPlaceholderColor(Color color);
    void     setMaxLength(int maxChars); //0 = unlimited
    void     setPassword(bool on);
    void     setPasswordChar(char c);
    void     insertText(const std::string& text);
    bool     scrollByPixels(int dx);
    void     adjustScroll();

    TextKeyMap&       keyMap()       { return keyMap_; }
    const TextKeyMap& keyMap() const { return keyMap_; }
    void setKeyBinding(TextEditAction action, SDL_Keycode key,
                       SDL_Keymod mod = SDL_KMOD_NONE)
    { keyMap_.set(action, key, mod); }
    void resetKeyBindings() { keyMap_.reset(); }
    const std::string& text() const { return text_; }
    const std::string& placeholder() const { return placeholder_; }
    int  maxLength() const { return maxLength_; }
    bool isPassword() const { return password_; }

    bool hasFocus() const { return hasFocus_; }
    int  cursorPosition() const { return cursorPos_; }
    bool hasSelection() const { return selAnchor_ >= 0 && selAnchor_ != cursorPos_; }
    bool textInputRect(Rect& rect) const override;

    void        selectAll();
    void        clearSelection();
    std::string selectedText() const;

    Widget* txt(const std::string& s)  { setText(s); return this; }
    Widget* clr(Color c)               { setTextColor(c); return this; }
    Widget* bgClr(Color c)             { setBackgroundColor(c); return this; }
    Widget* ph(const std::string& s)   { setPlaceholder(s); return this; }
    Widget* maxLen(int n)              { setMaxLength(n); return this; }
    Widget* pwd(bool on = true)        { setPassword(on); return this; }

protected:
    void onRender(SDL_Renderer* renderer) override;
    void onFontChanged() override;

private:
    friend class Application;
    int  positionAtX(int localX) const;
    bool deleteSelection();
    int  maskCharWidth() const;
    int  displayWidthTo(int bytePos) const;
    int  displayWidth() const;
    std::string displayText() const;
    void truncateToMaxLength();
    static int codepointCount(const std::string& text);

    std::string text_;
    Color       textColor_  = Color::Black;
    Color       bgColor_    = Color::White;
    Color       cursorColor_ = Color::Black;
    Color       selColor_   = Color(50, 100, 180);
    Color       borderColor_ = Color(140, 140, 140);
    std::string placeholder_;
    Color       placeholderColor_ = Color(130, 130, 130);
    int         maxLength_  = 0;
    bool        password_   = false;
    char        passwordChar_ = '*';
    int         cursorPos_  = 0;
    int         selAnchor_  = -1;   //-1 = no selection
    bool        hasFocus_   = false;
    bool        mouseSelecting_ = false;
    uint32_t    focusTick_  = 0;
    int         scrollOffX_ = 0;
    TextKeyMap  keyMap_;
};

//==============================================================================
// ListBox
//==============================================================================
class ListBox : public Widget
{
public:
    explicit ListBox(Widget* parent = nullptr);

    void addItem(const std::string& item);
    void removeItem(int index);
    void clear();
    int  selectedIndex() const { return selected_; }
    void setSelectedIndex(int index);
    const std::string& selectedItem() const;
    int  itemCount() const { return (int)items_.size(); }

    void setBackgroundColor(Color color);
    void setTextColor(Color color);
    void setSelectionColor(Color color);
    void setHoverColor(Color color);
    void setBorderColor(Color color);

    std::function<void(int)> onItemSelected;

    Widget* txt(const std::string& s) { addItem(s); return this; }

protected:
    void onRender(SDL_Renderer* renderer) override;
    void onMouseWheel(int delta) override;

private:
    std::vector<std::string> items_;
    int selected_ = -1;
    int hovered_  = -1;
    int scrollOffY_ = 0;
    bool draggingScrollbar_ = false;
    int  dragStartY_ = 0;
    int  dragStartOff_ = 0;
    int itemH_ = 20;
    Color textColor_ = Color::White;
    Color bgColor_ = Color(30, 30, 30);
    Color selColor_ = Color(40, 80, 140);
    Color hoverColor_ = Color(50, 50, 60);
    Color borderColor_ = Color(100, 100, 100);
};

//==============================================================================
// MenuBar
//==============================================================================
class MenuBar : public Widget
{
public:
    explicit MenuBar(Widget* parent = nullptr);

    int  addMenu(const std::string& title);
    int  addItem(int menuIdx, const std::string& text,
                 std::function<void()> onClick,
                 const std::string& shortcut = "");
    int  addSeparator(int menuIdx);
    void clearMenu(int menuIdx);
    void clearAll();

    void setBackgroundColor(Color color);
    void setTextColor(Color color);
    void setHoverColor(Color color);
    void setPopupColor(Color color);
    void setBorderColor(Color color);
    void setItemHoverColor(Color color);
    void setDisabledTextColor(Color color);
    void setShortcutColor(Color color);
    void setAccentColor(Color color);

    void closeAll();
    bool isMenuOpen() const { return openIdx_ >= 0; }

    std::function<void(int, int)> onItemSelected; // (menuIdx, itemIdx)

    void onRender(SDL_Renderer* renderer) override;
    void renderDropdownOverlay(SDL_Renderer* r);
    bool hitTest(int absX, int absY) const override;

private:
    static constexpr int itemH_ = 22;
    static constexpr int titlePad_ = 12;

    struct MenuItem
    {
        std::string text;
        std::string shortcut;
        std::function<void()> onClick;
        bool enabled = true;
        bool separator = false;
    };

    struct Menu
    {
        std::string title;
        std::vector<MenuItem> items;
    };

    int  titleAtX(int localX) const;
    int  dropdownLeft() const;
    int  dropdownWidth() const;
    int  dropdownHeight() const;
    Rect dropdownRect() const;
    void switchTo_(int idx);

    std::vector<Menu> menus_;
    int openIdx_ = -1;
    int hoveredTitle_ = -1;
    int hoveredItem_ = -1;
    bool switching_ = false; //true after opening via hover-on-another-title

    Color bgColor_ = Color(50, 50, 50);
    Color textColor_ = Color::White;
    Color hoverColor_ = Color(65, 65, 65);
    Color popupColor_ = Color(40, 40, 40);
    Color borderColor_ = Color(70, 70, 70);
    Color itemHoverColor_ = Color(50, 100, 180);
    Color disabledTextColor_ = Color(100, 100, 100);
    Color shortcutColor_ = Color(160, 160, 160);
    Color accentColor_ = Color(60, 120, 200);
};

//==============================================================================
// ContextMenuItem + ContextMenu
//==============================================================================
struct ContextMenuItem
{
    std::string text;
    std::function<void()> onClick;
    bool enabled = true;
    bool separator = false;
};

class ContextMenu : public Widget
{
public:
    explicit ContextMenu(Widget* parent = nullptr);

    void addItem(const std::string& text, std::function<void()> onClick);
    void addSeparator();
    void clear();
    void show(int screenX, int screenY);
    void dismiss();
    bool isActive() const { return active_; }
    int  contentHeight() const { return (int)items_.size() * 22; }

    void setBackgroundColor(Color color);
    void setTextColor(Color color);
    void setHoverColor(Color color);
    void setBorderColor(Color color);
    void setDisabledTextColor(Color color);
    void setSeparatorColor(Color color);

    std::function<void()> onDismiss;

    void renderContextMenu(SDL_Renderer* r) { if (active_) onRender(r); }

protected:
    void onRender(SDL_Renderer* renderer) override;
    bool hitTest(int absX, int absY) const override;

private:
    std::vector<ContextMenuItem> items_;
    int hoveredIdx_ = -1;
    int screenX_ = 0, screenY_ = 0;
    bool active_ = false;

    Color bgColor_ = Color(40, 40, 40);
    Color textColor_ = Color::White;
    Color hoverColor_ = Color(50, 100, 180);
    Color borderColor_ = Color(70, 70, 70);
    Color disabledTextColor_ = Color(100, 100, 100);
    Color separatorColor_ = Color(70, 70, 70);

    friend class Application;
};

//==============================================================================
// MessageBox
//==============================================================================
class MessageBox
{
public:
    MessageBox();

    void show(const char* title, const char* message,
              MessageBoxType type, MessageBoxButtons buttons,
              std::function<void(MessageBoxResult)> onResult,
              int windowW, int windowH);
    void dismiss();
    bool isActive() const { return active_; }

    //-1=overlay, -2=dialog interior, >=0=button index
    int  hitTestButton(int mx, int my) const;
    void renderOverlay(SDL_Renderer* renderer, int windowW, int windowH);

    void setOverlayColor(Color color);
    void setBackgroundColor(Color color);
    void setBorderColor(Color color);
    void setTextColor(Color color);
    void setTitleColors(Color background, Color text);
    void setButtonColors(Color normal, Color hover, Color border, Color text);
    void applyTheme(const Theme& theme);

private:
    struct BtnInfo
    {
        std::string      text;
        MessageBoxResult result;
        SDL_FRect        rect   = {};
        bool             hovered = false;
    };

    std::string  title_;
    std::string  message_;
    MessageBoxType   type_        = MessageBoxType::Info;
    std::vector<BtnInfo> buttons_;
    int                hoveredBtn_ = -1;
    int                pressedBtnIdx_ = -1;
    std::function<void(MessageBoxResult)> onResult_;
    bool               active_     = false;

    int dlgX_ = 0, dlgY_ = 0, dlgW_ = 300, dlgH_ = 150;

    Color overlayColor_ = Color(0, 0, 0, 180);
    Color bgColor_ = Color(45, 45, 45);
    Color borderColor_ = Color(70, 70, 70);
    Color textColor_ = Color::White;
    Color titleBgColor_ = Color(55, 55, 55);
    Color titleTextColor_ = Color::White;
    Color buttonBgColor_ = Color(60, 60, 60);
    Color buttonHoverColor_ = Color(85, 85, 85);
    Color buttonBorderColor_ = Color(100, 100, 100);
    Color buttonTextColor_ = Color::White;

    void calcLayout(int windowW, int windowH);

    friend class Application;
};

//==============================================================================
// Slider
//==============================================================================
class Slider : public Widget
{
public:
    explicit Slider(Widget* parent = nullptr);

    void setRange(float min, float max);
    void setValue(float val);
    float value() const { return value_; }
    float min()   const { return min_; }
    float max()   const { return max_; }

    void setTrackColor(Color color);
    void setFillColor(Color color);
    void setThumbColor(Color normal, Color hover);
    void setBorderColor(Color color);

    std::function<void(float)> onValueChanged;
    std::function<void(bool)> onDragChanged;

protected:
    void onRender(SDL_Renderer* renderer) override;

private:
    float min_ = 0.f, max_ = 100.f, value_ = 50.f;
    bool  dragging_ = false;
    Color trackColor_ = Color(60, 60, 60);
    Color fillColor_ = Color(80, 140, 200);
    Color thumbColor_ = Color(140, 140, 140);
    Color thumbHoverColor_ = Color(180, 180, 180);
    Color borderColor_ = Color(100, 100, 100);
};

//==============================================================================
// DropDown
//==============================================================================
class DropDown : public Widget
{
public:
    explicit DropDown(Widget* parent = nullptr);

    void addItem(const std::string& item);
    void clear();
    int  selectedIndex() const { return selected_; }
    void setSelectedIndex(int index);

    void setBackgroundColor(Color color);
    void setTextColor(Color color);
    void setBorderColor(Color color);
    void setArrowColor(Color color);
    void setPopupColor(Color color);
    void setSelectionColor(Color color);

    bool isPopupOpen() const override { return expanded_; }
    void closePopup() override
    {
        if (expanded_) { expanded_ = false; listH_ = 0; markDirty(); }
    }

    std::function<void(int)> onItemSelected;

protected:
    void onRender(SDL_Renderer* renderer) override;
    bool hitTest(int absX, int absY) const override;

private:
    std::vector<std::string> items_;
    int selected_ = -1;
    bool expanded_ = false;
    int listH_ = 0;
    Color bgColor_ = Color(50, 50, 50);
    Color textColor_ = Color::White;
    Color borderColor_ = Color(80, 80, 80);
    Color arrowColor_ = Color::Gray;
    Color popupColor_ = Color(30, 30, 35);
    Color selectionColor_ = Color(40, 80, 140);
};

//==============================================================================
// ComboBox
//==============================================================================
class ComboBox : public Widget
{
public:
    explicit ComboBox(Widget* parent = nullptr);

    void addItem(const std::string& item);
    void clear();
    int  selectedIndex() const { return selected_; }
    void setSelectedIndex(int index);
    void setText(const std::string& text);
    std::string text() const { return text_; }

    void setBackgroundColor(Color color);
    void setTextColor(Color color);
    void setBorderColor(Color color);
    void setButtonColor(Color color);
    void setArrowColor(Color color);
    void setPopupColor(Color color);
    void setSelectionColor(Color color);

    void insertText(const std::string& text);
    void backspace();
    void deleteForward();
    void moveCursorLeft();
    void moveCursorRight();
    void moveCursorHome();
    void moveCursorEnd();

    bool textInputRect(Rect& rect) const override;

    bool isPopupOpen() const override { return expanded_; }
    void closePopup() override
    {
        if (expanded_) { expanded_ = false; listH_ = 0; markDirty(); }
    }

    std::function<void(int)>    onItemSelected;
    std::function<void(const std::string&)> onTextChanged;

protected:
    void onRender(SDL_Renderer* renderer) override;
    bool hitTest(int absX, int absY) const override;

private:
    std::vector<std::string> items_;
    std::string text_;
    int selected_ = -1;
    int cursorPos_ = 0;
    bool expanded_ = false;
    int listH_ = 0;
    bool focused_ = false;
    Color bgColor_ = Color::White;
    Color textColor_ = Color::Black;
    Color borderColor_ = Color(140, 140, 140);
    Color buttonColor_ = Color(60, 60, 60);
    Color arrowColor_ = Color::Gray;
    Color popupColor_ = Color(30, 30, 35);
    Color selectionColor_ = Color(40, 80, 140);
};

//==============================================================================
// ScrollView
//==============================================================================
class ScrollView : public Widget
{
public:
    explicit ScrollView(Widget* parent = nullptr);

    void setContentSize(int w, int h);
    void setScrollOffset(int x, int y);
    int  scrollX() const { return scrollX_; }
    int  scrollY() const { return scrollY_; }

    void scrollbarV(bool show) { showVScroll_ = show; markDirty(); }
    void scrollbarH(bool show) { showHScroll_ = show; markDirty(); }
    void setScrollbarColors(Color track, Color thumb);

    ScrollView* sbV(bool s) { scrollbarV(s); return this; }
    ScrollView* sbH(bool s) { scrollbarH(s); return this; }

    bool hitTestScrollbar(int absX, int absY) const;
    bool draggingScrollbar() const { return sbDragging_; }

protected:
    void onRender(SDL_Renderer* renderer) override;
    bool hitTest(int absX, int absY) const override;
    void onMouseWheel(int delta) override;
    void renderScrollbars(SDL_Renderer* renderer);

    int scrollX_ = 0, scrollY_ = 0;
    int contentW_ = 0, contentH_ = 0;

private:
    bool sbDragging_ = false;
    int  sbDragMouse_ = 0;
    int  sbDragScroll_ = 0;
    bool sbVertical_ = false;
    bool showVScroll_ = false;
    bool showHScroll_ = false;
    Color scrollTrackColor_ = Color(40, 40, 40);
    Color scrollThumbColor_ = Color(100, 100, 100);
};

//==============================================================================
// TextArea — multi-line text display/editor
//==============================================================================
class TextArea : public ScrollView
{
public:
    explicit TextArea(Widget* parent = nullptr);

    void     setText(const std::string& text);
    void     setTextColor(Color color);
    void     setBackgroundColor(Color color);
    void     setCursorColor(Color color);
    void     setBorderColor(Color color);
    void     setReadOnly(bool ro);
    void     setSelectionColor(Color color);
    void     setLineSpacing(int px);   //extra pixels between lines
    void     setLetterSpacing(int px); //extra pixels between characters
    void     setShowLineNumbers(bool on) { showLineNumbers_ = on; refreshLayout(); markDirty(); }
    void     setAutoIndent(bool on) { autoIndent_ = on; }
    void     setSmartBackspace(bool on) { smartBackspace_ = on; }
    void     setIndentWithSpaces(bool on) { indentWithSpaces_ = on; }
    void     setTabWidth(int width)
    {
        tabWidth_ = std::max(1, std::min(width, 16));
    }
    int      lineSpacing() const { return lineSpacing_; }
    int      letterSpacing() const { return letterSpacing_; }
    bool     showLineNumbers() const { return showLineNumbers_; }
    bool     autoIndent() const { return autoIndent_; }
    bool     smartBackspace() const { return smartBackspace_; }
    bool     indentWithSpaces() const { return indentWithSpaces_; }
    int      tabWidth() const { return tabWidth_; }
    const std::string& text()     const { return text_; }
    Color    textColor()          const { return textColor_; }
    bool     readOnly()           const { return readOnly_; }

    bool        hasSelection() const;
    std::string selectedText() const;
    void        selectAll();
    void        clearSelection();
    bool        textInputRect(Rect& rect) const override;

    TextKeyMap&       keyMap()       { return keyMap_; }
    const TextKeyMap& keyMap() const { return keyMap_; }
    void setKeyBinding(TextEditAction action, SDL_Keycode key,
                       SDL_Keymod mod = SDL_KMOD_NONE)
    { keyMap_.set(action, key, mod); }
    void resetKeyBindings() { keyMap_.reset(); }

    //editing (used by Application, also available to callers)
    void setCursor(int line, int col);
    int  cursorLine() const   { return cursorLine_; }
    int  cursorColumn() const { return cursorCol_; }

    void insertText(const std::string& text);
    void insertNewline();
    void backspace();
    void deleteForward();
    void moveCursorLeft(bool select = false);
    void moveCursorRight(bool select = false);
    void moveCursorWordLeft(bool select = false);
    void moveCursorWordRight(bool select = false);
    void moveCursorUp(bool select = false);
    void moveCursorDown(bool select = false);
    void moveCursorHome(bool select = false);
    void moveCursorEnd(bool select = false);

    Widget* txt(const std::string& s) { setText(s); return this; }
    Widget* clr(Color c)              { setTextColor(c); return this; }

protected:
    void onRender(SDL_Renderer* renderer) override;
    void onFontChanged() override;

private:
    friend class Application;
    std::string text_;
    Color       textColor_ = Color::White;
    Color       bgColor_   = Color(25, 25, 28);
    Color       cursorColor_ = Color(200, 200, 200);
    Color       selColor_  = Color(50, 100, 180, 180);
    Color       borderColor_ = Color(70, 70, 75);
    int         lineSpacing_ = 0;
    int         letterSpacing_ = 0;
    bool        showLineNumbers_ = false;
    bool        autoIndent_ = false;
    bool        smartBackspace_ = false;
    bool        indentWithSpaces_ = true;
    int         tabWidth_ = 4;
    bool        readOnly_  = false;
    int         cursorLine_ = 0, cursorCol_ = 0;
    int         selAnchorLine_ = -1, selAnchorCol_ = -1;
    bool        mouseSelecting_ = false;
    bool        cursorVisible_ = false;
    uint32_t    cursorTick_ = 0;
    TextKeyMap  keyMap_;

    std::vector<std::string> splitLines() const;
    std::size_t offsetAt(int line, int col) const;
    void        clampCursor();
    void        refreshLayout();
    void        ensureCursorVisible();
    bool        deleteSelection();
    void        selectionBounds(int& sl, int& sc, int& el, int& ec) const;
    void        lineColAt(int localX, int localY, int& line, int& col) const;
    void        lineColForOffset(std::size_t off, int& line, int& col) const;
    static bool isWordCodepoint(uint32_t cp);
    int         lineHeight() const;
    int         glyphAdvance(uint32_t codepoint) const;
    int         prefixWidth(const std::string& line, int bytePos) const;
    int         lineWidth(const std::string& line) const;
    int         lineNumberGutterWidth() const;
    void        drawLine(SDL_Renderer* renderer, const std::string& line,
                         int x, int y, Color color) const;
};

//==============================================================================
// Window — OS-level window
//==============================================================================
class Window
{
public:
    enum class TitleBarMode { System, Custom, Hidden };

    Window(const char* title, int w, int h,
           TitleBarMode mode = TitleBarMode::System);
    ~Window();

    void     setTitle(const char* title);
    void     setTitleBarMode(TitleBarMode mode);
    void     setTitleBarHeight(int h);
    void     setBackgroundColor(Color color);
    void     setTitleBarColor(Color color);
    void     setTitleTextColor(Color color);
    void     setTitleButtonColors(Color normal, Color hover);
    void     setTitleCloseColor(Color color);
    void     setTheme(const Theme& theme);
    void     setResizable(bool resiz);
    void     requestRedraw() { redraw_ = true; }
    bool     needsRedraw() const;

    bool               valid()           const { return sdlWindow_ != nullptr && renderer_ != nullptr; }

    const std::string& title()           const { return title_; }
    TitleBarMode       titleBarMode()    const { return titleBarMode_; }
    int                titleBarHeight()  const { return titleBarHeight_; }
    int                clientWidth()     const { return clientWidth_; }
    int                clientHeight()    const { return clientHeight_; }
    bool               resizable()       const { return resizable_; }
    const Theme&       theme()           const { return theme_; }
    SDL_Window*        handle()          const { return sdlWindow_; }
    SDL_Renderer*      renderer()        const { return renderer_; }
    Widget&            root()                  { return *root_; }

    void render();

    void showMessageBox(const char* title, const char* message,
                        MessageBoxType type, MessageBoxButtons buttons,
                        std::function<void(MessageBoxResult)> onResult);

    MessageBoxResult showMessageBoxBlocking(const char* title, const char* message,
                                             MessageBoxType type, MessageBoxButtons buttons);

private:
    friend class Application;
    static SDL_HitTestResult SDLCALL hitTest(SDL_Window* window,
                                             const SDL_Point* area, void* data);

    SDL_Window*   sdlWindow_       = nullptr;
    SDL_Renderer* renderer_        = nullptr;
    std::string   title_;
    Theme         theme_;
    Color         bgColor_         = Color(30, 30, 30);
    Color         titleBarColor_   = Color(50, 50, 50);
    Color         titleTextColor_  = Color::White;
    Color         titleButtonColor_ = Color(50, 50, 50);
    Color         titleButtonHoverColor_ = Color(80, 80, 80);
    Color         titleCloseColor_ = Color(180, 30, 30);
    TitleBarMode  titleBarMode_    = TitleBarMode::System;
    int           titleBarHeight_  = 0;
    int           clientWidth_     = 800;
    int           clientHeight_    = 600;
    bool          resizable_       = true;
    bool          shown_           = false;
    bool          redraw_          = true;
    uint64_t      lastBlinkPhase_  = 0;
    std::unique_ptr<Widget> root_;

    //custom title bar state
    bool          dragging_        = false;
    int           dragStartWinX_   = 0;
    int           dragStartWinY_   = 0;
    float         dragStartMouseX_ = 0.f;
    float         dragStartMouseY_ = 0.f;
    bool          maximized_       = false;
    int           hoveredBtn_      = 0;  //0=none 1=min 2=max 3=close
    int           pressedTitleBtn_ = 0;  //button pressed on mouse down

    void onResized(int w, int h);
    bool handleTitleBarEvent(const SDL_Event& ev, int mx, int my);
    void forgetWidget(Widget* w);
    bool anyContinuousRedraw(Widget* w) const;

    //context menu
    std::unique_ptr<ContextMenu> contextMenu_;

    //message box
    std::unique_ptr<MessageBox> messageBox_;

    //menu bar
    class MenuBar* menuBar_ = nullptr;

    //keyboard focus owned by this window
    Widget* focused_ = nullptr;
    bool    focusRing_ = false;

    friend class MenuBar;
    friend class Widget;
};

//==============================================================================
// Application
//==============================================================================
class Application
{
public:
    Application();
    ~Application();

    Window& addWindow(const char* title, int w, int h);
    void    run();
    void    quit();
    void    refreshFont();
    void    setTheme(const Theme& theme);

    void    setFPS(int fps);
    int     fps() const { return targetFps_; }

    //Tab / Shift+Tab focus traversal is opt-in (off by default).
    void    setTabTraversal(bool enabled) { tabTraversal_ = enabled; }
    bool    tabTraversal() const { return tabTraversal_; }

    bool    isRunning() const { return running_; }
    bool    valid()     const { return initialized_; }
    Widget* focused()  const { return focused_; }
    void    setFocus(Widget* w, bool fromKeyboard = false);

    std::function<void(const SDL_Event&)> onUserEvent;

    void processEvents();

    static Application* instance() { return instance_; }

private:
    bool running_ = false;
    bool initialized_ = false;
    bool tabTraversal_ = false;
    int  targetFps_ = 60;
    std::vector<std::unique_ptr<Window>> windows_;
    Widget* focused_  = nullptr;
    Widget* hovered_  = nullptr;
    Widget* pressedWidget_ = nullptr;

    static Application* instance_;

    Window* findWindowByID(Uint32 id);
    Widget* hitTest(Widget* w, int absX, int absY);
    void    updateHovered(Widget* target);
    void    updateTextInputArea(Widget* widget);
    void    widgetDestroyed(Widget* w);
    void    closePopups(Widget* w, int absX, int absY);
    void    processEvent(const SDL_Event& ev);
    void    handleMouseEvent(const SDL_Event& ev);
    void    handleKeyEvent(const SDL_Event& ev);

    friend class Widget;
};

//==============================================================================
// Implementation
//==============================================================================
#ifdef SDLGUI_IMPLEMENTATION

//==============================================================================
// TextKeyMap
//==============================================================================
TextKeyMap::TextKeyMap()
{
    reset();
}

uint16_t TextKeyMap::normalize(SDL_Keymod mod)
{
    return (uint16_t)(mod & (SDL_KMOD_SHIFT | SDL_KMOD_CTRL |
                             SDL_KMOD_ALT | SDL_KMOD_GUI));
}

void TextKeyMap::clear()
{
    bindings_.clear();
}

void TextKeyMap::set(TextEditAction action, SDL_Keycode key, SDL_Keymod mod)
{
    if (key == SDLK_UNKNOWN)
    {
        remove(action);
        return;
    }

    uint16_t m = normalize(mod);
    bindings_.erase(
        std::remove_if(bindings_.begin(), bindings_.end(),
            [action, key, m](const Binding& b)
            {
                return b.action == action || (b.key == key && b.mod == m);
            }),
        bindings_.end());

    bindings_.push_back({ action, key, m });
}

void TextKeyMap::add(TextEditAction action, SDL_Keycode key, SDL_Keymod mod)
{
    if (key == SDLK_UNKNOWN) return;

    uint16_t m = normalize(mod);
    bindings_.erase(
        std::remove_if(bindings_.begin(), bindings_.end(),
            [key, m](const Binding& b)
            {
                return b.key == key && b.mod == m;
            }),
        bindings_.end());

    bindings_.push_back({ action, key, m });
}

void TextKeyMap::remove(TextEditAction action)
{
    bindings_.erase(
        std::remove_if(bindings_.begin(), bindings_.end(),
            [action](const Binding& b) { return b.action == action; }),
        bindings_.end());
}

TextEditAction TextKeyMap::find(SDL_Keycode key, SDL_Keymod mod) const
{
    uint16_t m = normalize(mod);
    for (const auto& b : bindings_)
    {
        if (b.key == key && b.mod == m)
            return b.action;
    }
    return TextEditAction::None;
}

void TextKeyMap::reset()
{
    clear();

    set(TextEditAction::CursorLeft,  SDLK_LEFT,  SDL_KMOD_NONE);
    set(TextEditAction::CursorRight, SDLK_RIGHT, SDL_KMOD_NONE);
    set(TextEditAction::CursorUp,    SDLK_UP,    SDL_KMOD_NONE);
    set(TextEditAction::CursorDown,  SDLK_DOWN,  SDL_KMOD_NONE);
    set(TextEditAction::CursorHome,  SDLK_HOME,  SDL_KMOD_NONE);
    set(TextEditAction::CursorEnd,   SDLK_END,   SDL_KMOD_NONE);

    set(TextEditAction::SelectLeft,  SDLK_LEFT,  SDL_KMOD_SHIFT);
    set(TextEditAction::SelectRight, SDLK_RIGHT, SDL_KMOD_SHIFT);
    set(TextEditAction::SelectUp,    SDLK_UP,    SDL_KMOD_SHIFT);
    set(TextEditAction::SelectDown,  SDLK_DOWN,  SDL_KMOD_SHIFT);
    set(TextEditAction::SelectHome,  SDLK_HOME,  SDL_KMOD_SHIFT);
    set(TextEditAction::SelectEnd,   SDLK_END,   SDL_KMOD_SHIFT);

    set(TextEditAction::SelectAll, SDLK_A, SDL_KMOD_CTRL);
    set(TextEditAction::Copy,      SDLK_C, SDL_KMOD_CTRL);
    set(TextEditAction::Cut,       SDLK_X, SDL_KMOD_CTRL);
    set(TextEditAction::Paste,     SDLK_V, SDL_KMOD_CTRL);

    set(TextEditAction::Backspace,     SDLK_BACKSPACE, SDL_KMOD_NONE);
    set(TextEditAction::DeleteForward, SDLK_DELETE,    SDL_KMOD_NONE);
    set(TextEditAction::InsertNewline, SDLK_RETURN,    SDL_KMOD_NONE);
    add(TextEditAction::InsertNewline, SDLK_KP_ENTER,  SDL_KMOD_NONE);
    set(TextEditAction::ClearSelection, SDLK_ESCAPE,   SDL_KMOD_NONE);
}

//==============================================================================
// Color constants
//==============================================================================
const Color Color::Black       {   0,   0,   0 };
const Color Color::White       { 255, 255, 255 };
const Color Color::Red         { 255,   0,   0 };
const Color Color::Green       {   0, 255,   0 };
const Color Color::Blue        {   0,   0, 255 };
const Color Color::Gray        { 128, 128, 128 };
const Color Color::LightGray   { 192, 192, 192 };
const Color Color::Transparent {   0,   0,   0,   0 };

Application* Application::instance_ = nullptr;

//==============================================================================
// Font state
//==============================================================================
namespace detail {

static std::shared_ptr<Font> activeFont_;
static uint64_t fontEpoch_ = 1;

} //namespace detail

namespace detail {

static uint32_t decodeUTF8(const char*& p)
{
    uint32_t cp = (unsigned char)*p;
    if (cp < 0x80) { p++; return cp; }
    if ((cp & 0xE0) == 0xC0 && (p[1] & 0xC0) == 0x80)
    {
        cp = ((cp & 0x1F) << 6) | (unsigned char)(p[1] & 0x3F);
        p += 2; return cp;
    }
    if ((cp & 0xF0) == 0xE0 && (p[1] & 0xC0) == 0x80 && (p[2] & 0xC0) == 0x80)
    {
        cp = ((cp & 0x0F) << 12) | ((unsigned char)(p[1] & 0x3F) << 6)
           | (unsigned char)(p[2] & 0x3F);
        p += 3; return cp;
    }
    if ((cp & 0xF8) == 0xF0 && (p[1] & 0xC0) == 0x80
        && (p[2] & 0xC0) == 0x80 && (p[3] & 0xC0) == 0x80)
    {
        cp = ((cp & 0x07) << 18) | ((unsigned char)(p[1] & 0x3F) << 12)
           | ((unsigned char)(p[2] & 0x3F) << 6) | (unsigned char)(p[3] & 0x3F);
        p += 4; return cp;
    }
    p++; return 0xFFFD;
}

static int utf8CharLen(const char* s)
{
    uint8_t c = (uint8_t)s[0];
    if (c < 0x80) return 1;
    if ((c & 0xE0) == 0xC0) return 2;
    if ((c & 0xF0) == 0xE0) return 3;
    if ((c & 0xF8) == 0xF0) return 4;
    return 1;
}

static int utf8Prev(const std::string& s, int pos)
{
    if (pos <= 0) return 0;
    int st = pos - 1;
    while (st > 0 && ((uint8_t)s[st] & 0xC0) == 0x80)
        st--;
    return st;
}

//Saves the renderer clip on construction and restores it on destruction.
class ClipGuard
{
public:
    ClipGuard(SDL_Renderer* renderer, int x, int y, int w, int h)
        : renderer_(renderer)
    {
        SDL_Rect clip = { x, y, w, h };
        if (SDL_RenderClipEnabled(renderer_))
        {
            SDL_Rect prev = {};
            if (SDL_GetRenderClipRect(renderer_, &prev))
            {
                hasPrev_ = true;
                prev_ = prev;
                clip = intersect(clip, prev);
            }
        }
        SDL_SetRenderClipRect(renderer_, &clip);
    }

    ~ClipGuard()
    {
        SDL_SetRenderClipRect(renderer_, hasPrev_ ? &prev_ : nullptr);
    }

    ClipGuard(const ClipGuard&) = delete;
    ClipGuard& operator=(const ClipGuard&) = delete;

private:
    static SDL_Rect intersect(const SDL_Rect& a, const SDL_Rect& b)
    {
        int x0 = std::max(a.x, b.x);
        int y0 = std::max(a.y, b.y);
        int x1 = std::min(a.x + a.w, b.x + b.w);
        int y1 = std::min(a.y + a.h, b.y + b.h);
        if (x1 <= x0 || y1 <= y0) return { 0, 0, 0, 0 };
        return { x0, y0, x1 - x0, y1 - y0 };
    }

    SDL_Renderer* renderer_;
    SDL_Rect      prev_ = {};
    bool          hasPrev_ = false;
};

} //namespace detail

namespace detail {
static int glyphWidth(uint32_t codepoint);
static int textWidth(const char* text);
static int textWidthTo(const char* text, int bytePos);
static void drawText(SDL_Renderer* renderer, const char* text,
                     int x, int y, Color color);
} //namespace detail

#ifdef SDLGUI_USE_BUILTIN_FONT
#include "fontData.hpp"

namespace detail {

constexpr auto font8CharCount  = 256u;
constexpr auto font8Height     = 16;
constexpr auto font16CharCount = 65533u;
constexpr auto font16Height    = 32;

static bool hasGlyph8(uint32_t cp)
{
    static bool ready = false;
    static bool present[font8CharCount];

    if (!ready)
    {
        for (uint32_t c = 0; c < font8CharCount; ++c)
        {
            if (c == 0x20) { present[c] = true; continue; }
            for (auto r = 0; r < font8Height; ++r)
                if (font8x16[c][r]) { present[c] = true; break; }
        }
        ready = true;
    }

    return cp < font8CharCount && present[cp];
}

static int builtinGlyphWidth(uint32_t cp)
{
    if (cp < font8CharCount && hasGlyph8(cp)) return 8;
    if (cp < font16CharCount) return 16;
    return 8;
}

static int builtinTextWidth(const char* s)
{
    int w = 0;
    const char* p = s;
    while (*p)
    {
        uint32_t cp = decodeUTF8(p);
        w += builtinGlyphWidth(cp);
    }
    return w;
}

static int builtinTextWidthTo(const char* s, int bytePos)
{
    int w = 0;
    const char* p = s;
    int consumed = 0;
    while (*p && consumed < bytePos)
    {
        const char* prev = p;
        uint32_t cp = decodeUTF8(p);
        int bytes = (int)(p - prev);
        if (consumed + bytes > bytePos) break;
        consumed += bytes;
        w += builtinGlyphWidth(cp);
    }
    return w;
}

//Split text into lines, wrapping at character boundaries when needed.
//maxWidth <= 0 wraps after every character; use a huge value to disable wrapping.
static void wrapText(const char* text, int maxWidth, std::vector<std::string>& out)
{
    out.clear();

    std::string line;
    int lineW = 0;
    const char* p = text ? text : "";

    while (*p)
    {
        if (*p == '\n')
        {
            out.push_back(line);
            line.clear();
            lineW = 0;
            ++p;
            continue;
        }

        const char* start = p;
        uint32_t cp = decodeUTF8(p);
        int cw = glyphWidth(cp);

        if (lineW + cw > maxWidth && !line.empty())
        {
            out.push_back(line);
            line.clear();
            lineW = 0;
        }

        line.append(start, p - start);
        lineW += cw;
    }

    out.push_back(line);
}

//==============================================================================
// Text texture cache
//==============================================================================
struct TextCacheKey
{
    std::string text;
    uint32_t    color = 0;
    uint64_t    fontEpoch = 0;

    bool operator==(const TextCacheKey& other) const
    {
        return color == other.color && fontEpoch == other.fontEpoch &&
               text == other.text;
    }
};

struct TextCacheKeyHash
{
    std::size_t operator()(const TextCacheKey& k) const
    {
        std::size_t h = std::hash<std::string>()(k.text);
        h ^= (std::size_t(k.color) * 2654435761u);
        h ^= (std::size_t(k.fontEpoch) * 2246822519u);
        return h;
    }
};

struct TextCacheEntry
{
    SDL_Texture* texture = nullptr;
    uint64_t     lastUse = 0;
};

struct TextCache
{
    std::unordered_map<TextCacheKey, TextCacheEntry, TextCacheKeyHash> entries;
    uint64_t tick = 0;
};

static const std::size_t maxCachedTextures = 512;
static std::unordered_map<SDL_Renderer*, TextCache> textCaches;

static SDL_Texture* createTextTexture(SDL_Renderer* renderer, const char* text,
                                      int totalW, Color color)
{
    SDL_Surface* surf = SDL_CreateSurface(
        totalW, 16, SDL_PIXELFORMAT_RGBA8888);
    if (!surf) return nullptr;

    SDL_FillSurfaceRect(surf, nullptr, SDL_MapSurfaceRGBA(surf, 0, 0, 0, 0));
    auto pixels = static_cast<uint32_t*>(surf->pixels);
    uint32_t col = SDL_MapSurfaceRGBA(surf, color.r, color.g, color.b, color.a);

    int curX = 0;
    const char* p = text;
    while (*p)
    {
        uint32_t cp = decodeUTF8(p);

        if (cp < font8CharCount && hasGlyph8(cp))
        {
            for (auto row = 0; row < font8Height; row++)
            {
                uint8_t bits = font8x16[cp][row];
                for (int cb = 0; cb < 8; cb++)
                    if (bits & (1 << (7 - cb)))
                        pixels[row * totalW + curX + cb] = col;
            }
            curX += 8;
        }
        else if (cp < font16CharCount)
        {
            for (auto row = 0; row < font8Height; row++)
            {
                uint8_t b0 = font16x16[cp][row * 2];
                uint8_t b1 = font16x16[cp][row * 2 + 1];
                for (int cb = 0; cb < 8; cb++)
                {
                    if (b0 & (1 << (7 - cb)))
                        pixels[row * totalW + curX + cb] = col;
                    if (b1 & (1 << (7 - cb)))
                        pixels[row * totalW + curX + 8 + cb] = col;
                }
            }
            curX += 16;
        }
        else
        {
            curX += 8; //unknown codepoint, skip
        }
    }

    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_DestroySurface(surf);
    return tex;
}

static void evictOldestText(TextCache& cache)
{
    if (cache.entries.empty()) return;

    auto oldest = cache.entries.begin();
    for (auto it = cache.entries.begin(); it != cache.entries.end(); ++it)
    {
        if (it->second.lastUse < oldest->second.lastUse)
            oldest = it;
    }

    if (oldest->second.texture)
        SDL_DestroyTexture(oldest->second.texture);

    cache.entries.erase(oldest);
}

static void clearTextCache(SDL_Renderer* renderer)
{
    auto it = textCaches.find(renderer);
    if (it == textCaches.end()) return;

    for (auto& kv : it->second.entries)
    {
        if (kv.second.texture)
            SDL_DestroyTexture(kv.second.texture);
    }

    textCaches.erase(it);
}

static void clearAllTextCaches()
{
    for (auto& cache : textCaches)
    {
        for (auto& kv : cache.second.entries)
        {
            if (kv.second.texture)
                SDL_DestroyTexture(kv.second.texture);
        }
    }
    textCaches.clear();
}

static void drawBuiltinText(SDL_Renderer* renderer, const char* text,
                            int x, int y, Color color)
{
    if (!text || !*text) return;

    int totalW = builtinTextWidth(text);
    if (totalW <= 0) return;

    TextCache& cache = textCaches[renderer];
    TextCacheKey key{ text, color.toUint32(), fontEpoch_ };

    SDL_Texture* tex = nullptr;
    auto it = cache.entries.find(key);

    if (it != cache.entries.end())
    {
        tex = it->second.texture;
        it->second.lastUse = ++cache.tick;
    }
    else
    {
        tex = createTextTexture(renderer, text, totalW, color);
        if (!tex) return;

        if (cache.entries.size() >= maxCachedTextures)
            evictOldestText(cache);

        TextCacheEntry entry;
        entry.texture = tex;
        entry.lastUse = ++cache.tick;
        cache.entries.emplace(std::move(key), entry);
    }

    SDL_FRect dst = { (float)x, (float)y, (float)totalW, 16.0f };
    SDL_RenderTexture(renderer, tex, nullptr, &dst);
}

class BitmapFont : public Font
{
public:
    int lineHeight() const override { return 16; }
    int ascent() const override { return 16; }

    int measure(const char* text) const override
    {
        return builtinTextWidth(text);
    }

    int measureTo(const char* text, int bytePos) const override
    {
        return builtinTextWidthTo(text, bytePos);
    }

    int advance(uint32_t codepoint) const override
    {
        return builtinGlyphWidth(codepoint);
    }

    void draw(SDL_Renderer* renderer, const char* text,
              int x, int y, Color color) override
    {
        drawBuiltinText(renderer, text, x, y, color);
    }
};

} //namespace detail

#else //not SDLGUI_USE_BUILTIN_FONT

namespace detail {

class FallbackFont : public Font
{
public:
    int measure(const char* text) const override
    {
        return (int)std::strlen(text ? text : "") * 8;
    }

    int measureTo(const char* text, int bytePos) const override
    {
        return std::min(bytePos, (int)std::strlen(text ? text : "")) * 8;
    }

    int advance(uint32_t) const override { return 8; }

    void draw(SDL_Renderer*, const char*, int, int, Color) override {}
};

static void wrapText(const char* text, int maxWidth, std::vector<std::string>& out)
{
    out.clear();

    std::string line;
    int lineW = 0;
    const char* p = text ? text : "";

    while (*p)
    {
        if (*p == '\n')
        {
            out.push_back(line);
            line.clear();
            lineW = 0;
            ++p;
            continue;
        }

        const char* start = p;
        uint32_t cp = decodeUTF8(p);
        int cw = glyphWidth(cp);

        if (lineW + cw > maxWidth && !line.empty())
        {
            out.push_back(line);
            line.clear();
            lineW = 0;
        }

        line.append(start, p - start);
        lineW += cw;
    }

    out.push_back(line);
}

static void clearTextCache(SDL_Renderer*) {}
static void clearAllTextCaches() {}

} //namespace detail

#endif //SDLGUI_USE_BUILTIN_FONT

//==============================================================================
// Font dispatch
//==============================================================================
namespace detail {

static int glyphWidth(uint32_t codepoint)
{
    return currentFont()->advance(codepoint);
}

static int textWidth(const char* text)
{
    return currentFont()->measure(text);
}

static int textWidthTo(const char* text, int bytePos)
{
    return currentFont()->measureTo(text, bytePos);
}

static void drawText(SDL_Renderer* renderer, const char* text,
                     int x, int y, Color color)
{
    if (!text || !*text) return;
    currentFont()->draw(renderer, text, x, y, color);
}

} //namespace detail

CallbackFont::CallbackFont(FontMeasure measure, FontMeasureTo measureTo,
                           FontAdvance advance, FontDraw draw,
                           int lineHeight, int ascent)
    : measure_(std::move(measure)),
      measureTo_(std::move(measureTo)),
      advance_(std::move(advance)),
      draw_(std::move(draw)),
      lineHeight_(lineHeight),
      ascent_(ascent)
{
}

int CallbackFont::measure(const char* text) const
{
    if (measure_) return measure_(text);

    int w = 0;
    const char* p = text ? text : "";
    while (*p)
    {
        uint32_t cp = detail::decodeUTF8(p);
        w += advance_ ? advance_(cp) : 8;
    }
    return w;
}

int CallbackFont::measureTo(const char* text, int bytePos) const
{
    if (measureTo_) return measureTo_(text, bytePos);

    int w = 0;
    int consumed = 0;
    const char* p = text ? text : "";
    while (*p && consumed < bytePos)
    {
        const char* prev = p;
        uint32_t cp = detail::decodeUTF8(p);
        int bytes = (int)(p - prev);
        if (consumed + bytes > bytePos) break;
        consumed += bytes;
        w += advance_ ? advance_(cp) : 8;
    }
    return w;
}

int CallbackFont::advance(uint32_t codepoint) const
{
    return advance_ ? advance_(codepoint) : 8;
}

void CallbackFont::draw(SDL_Renderer* renderer, const char* text,
                        int x, int y, Color color)
{
    if (draw_) draw_(renderer, text, x, y, color);
}

std::shared_ptr<Font> defaultFont()
{
#ifdef SDLGUI_USE_BUILTIN_FONT
    static std::shared_ptr<Font> font = std::make_shared<detail::BitmapFont>();
#else
    static std::shared_ptr<Font> font = std::make_shared<detail::FallbackFont>();
#endif
    return font;
}

Font* currentFont()
{
    return detail::activeFont_ ? detail::activeFont_.get()
                               : defaultFont().get();
}

uint64_t fontEpoch()
{
    return detail::fontEpoch_;
}

void setFont(std::shared_ptr<Font> font)
{
    detail::activeFont_ = std::move(font);
    ++detail::fontEpoch_;
    detail::clearAllTextCaches();

    if (auto* app = Application::instance())
        app->refreshFont();
}

void setFontRenderer(FontRenderer fn)
{
#ifdef SDLGUI_USE_BUILTIN_FONT
    (void)fn;
    //Use setFont() to override the built-in bitmap font at runtime.
#else
    if (!fn)
    {
        setFont(nullptr);
        return;
    }

    FontDraw draw = [fn](SDL_Renderer* renderer, const char* text,
                         int x, int y, Color color)
    {
        fn(renderer, text, x, y, color);
    };

    setFont(std::make_shared<CallbackFont>(
        FontMeasure{}, FontMeasureTo{}, FontAdvance{}, draw, 16, 16));
#endif
}

//==============================================================================
// Widget
//==============================================================================
Widget::Widget(Widget* parent)
    : parent_(parent)
{
    if (parent)
        window_ = parent->window_;
}

Widget::~Widget()
{
    if (window_)
        window_->forgetWidget(this);

    if (auto* app = Application::instance())
        app->widgetDestroyed(this);
}

void Widget::setPosition(int x, int y)
{
    x_ = x;
    y_ = y;
    markDirty();
}

void Widget::setSize(int w, int h)
{
    if (w < 0) w = 0;
    if (h < 0) h = 0;
    if (w == width_ && h == height_) return;

    width_  = w;
    height_ = h;
    markDirty();

    relayout();
    if (parent_ && !parent_->layingOut_)
        parent_->relayout();
}

void Widget::setAnchors(int flags)
{
    int pw = parent_ ? parent_->width_ : 0;
    int ph = parent_ ? parent_->height_ : 0;

    anchorL_ = x_;
    anchorT_ = y_;
    anchorR_ = pw - x_ - width_;
    anchorB_ = ph - y_ - height_;
    anchorFlags_ = (uint8_t)flags;

    if (parent_)
        parent_->relayout();
}

void Widget::setAnchors(int flags, int left, int top, int right, int bottom)
{
    anchorFlags_ = (uint8_t)flags;
    anchorL_ = left;
    anchorT_ = top;
    anchorR_ = right;
    anchorB_ = bottom;

    if (parent_)
        parent_->relayout();
}

void Widget::setAnchorMargins(int left, int top, int right, int bottom)
{
    anchorL_ = left;
    anchorT_ = top;
    anchorR_ = right;
    anchorB_ = bottom;

    if (parent_)
        parent_->relayout();
}

void Widget::setStretch(float weight)
{
    if (weight < 0.0f) weight = 0.0f;
    if (layoutStretch_ == weight) return;
    layoutStretch_ = weight;
    if (parent_ && !parent_->layingOut_)
        parent_->relayout();
}

void Widget::setFillCross(bool on)
{
    if (fillCross_ == on) return;
    fillCross_ = on;
    if (parent_ && !parent_->layingOut_)
        parent_->relayout();
}

void Widget::setBoundsInternal(int x, int y, int w, int h)
{
    if (w < 0) w = 0;
    if (h < 0) h = 0;

    bool sizeChanged = (w != width_ || h != height_);
    x_ = x;
    y_ = y;
    width_ = w;
    height_ = h;
    markDirty();

    if (sizeChanged)
        relayout();
}

void Widget::relayout()
{
    if (layingOut_) return;
    layingOut_ = true;
    layoutChildren();
    layingOut_ = false;
}

void Widget::layoutChildren()
{
    for (auto& child : children_)
    {
        if (!child->anchorFlags_) continue;

        int x = child->x_;
        int y = child->y_;
        int w = child->width_;
        int h = child->height_;
        int pw = width_;
        int ph = height_;

        int hFlags = child->anchorFlags_ & (AnchorLeft | AnchorRight);
        if (hFlags == (AnchorLeft | AnchorRight))
        {
            x = child->anchorL_;
            w = pw - child->anchorL_ - child->anchorR_;
        }
        else if (hFlags == AnchorLeft)
        {
            x = child->anchorL_;
        }
        else if (hFlags == AnchorRight)
        {
            x = pw - w - child->anchorR_;
        }

        int vFlags = child->anchorFlags_ & (AnchorTop | AnchorBottom);
        if (vFlags == (AnchorTop | AnchorBottom))
        {
            y = child->anchorT_;
            h = ph - child->anchorT_ - child->anchorB_;
        }
        else if (vFlags == AnchorTop)
        {
            y = child->anchorT_;
        }
        else if (vFlags == AnchorBottom)
        {
            y = ph - h - child->anchorB_;
        }

        child->setBoundsInternal(x, y, w, h);
    }
}

int Widget::absoluteX() const
{
    int base = parent_ ? parent_->absoluteX() + x_ : x_;
    return base + renderOffX_;
}

int Widget::absoluteY() const
{
    int base = parent_ ? parent_->absoluteY() + y_ : y_;
    return base + renderOffY_;
}

Rect Widget::absoluteRect() const
{
    return { absoluteX(), absoluteY(), width_, height_ };
}

void Widget::addChild(std::unique_ptr<Widget> child)
{
    if (!child) return;
    child->parent_ = this;
    child->setWindowRecursive(window_);
    Widget* raw = child.get();
    children_.push_back(std::move(child));
    raw->relayout();
    relayout();
    markDirty();
}

void Widget::removeChild(Widget* child)
{
    auto it = std::find_if(children_.begin(), children_.end(),
        [child](const std::unique_ptr<Widget>& c) { return c.get() == child; });
    if (it != children_.end())
    {
        children_.erase(it);
        relayout();
        markDirty();
    }
}

void Widget::setWindowRecursive(Window* w)
{
    window_ = w;
    for (auto& child : children_)
        child->setWindowRecursive(w);
}

void Widget::setVisible(bool v)
{
    if (visible_ == v) return;
    visible_ = v;
    markDirty();
    if (parent_ && !parent_->layingOut_)
        parent_->relayout();
}

void Widget::setEnabled(bool e)
{
    enabled_ = e;
    markDirty();
}

void Widget::notifyFontChanged()
{
    onFontChanged();
    for (auto& child : children_)
        child->notifyFontChanged();
}

void Widget::onRender(SDL_Renderer* renderer)
{
    if (!visible_) return;

    Rect r = absoluteRect();
    detail::ClipGuard clip(renderer, r.x, r.y, r.w, r.h);

    for (auto& child : children_)
        child->onRender(renderer);

    dirty_ = false;
}

void Widget::markDirty()
{
    dirty_ = true;
    if (parent_)
        parent_->markDirty();
}

void Widget::requestRedraw()
{
    if (window_)
        window_->requestRedraw();
    else
        markDirty();
}

bool Widget::hitTest(int absX, int absY) const
{
    if (!visible_ || !enabled_) return false;
    int ax = absoluteX();
    int ay = absoluteY();
    return absX >= ax && absX < ax + width_ &&
           absY >= ay && absY < ay + height_;
}

void Widget::onMouseWheel(int delta)
{
    (void)delta;
}

//==============================================================================
// MenuBar
//==============================================================================
MenuBar::MenuBar(Widget* parent)
    : Widget(parent)
{
    height_ = 24;
    if (window_)
        window_->menuBar_ = this;

    onMouseMove = [this](MouseEvent& e)
    {
        int prevTitle = hoveredTitle_;
        int prevItem  = hoveredItem_;
        hoveredTitle_ = titleAtX(e.x);

        //if a menu is open and hovering over another title, switch
        if (openIdx_ >= 0 && hoveredTitle_ >= 0 && hoveredTitle_ != openIdx_)
        {
            switchTo_(hoveredTitle_);
        }

        //hover over dropdown items
        if (openIdx_ >= 0)
        {
            Rect dr = dropdownRect();
            int localY = e.y - (dr.y - absoluteY());
            hoveredItem_ = (localY >= 0 && localY < dr.h)
                ? localY / itemH_
                : -1;
        }
        else
        {
            hoveredItem_ = -1;
        }

        if (prevTitle != hoveredTitle_ || prevItem != hoveredItem_)
            markDirty();
    };

    onMouseLeave = [this](MouseEvent&)
    {
        if (hoveredTitle_ != -1)
        {
            hoveredTitle_ = -1;
            markDirty();
        }
    };

    onMouseDown = [this](MouseEvent& e)
    {
        if (e.button != SDL_BUTTON_LEFT) return;

        //check if click is inside dropdown area
        Rect dr = dropdownRect();
        int ax = absoluteX(), ay = absoluteY();

        if (openIdx_ >= 0)
        {
            int sx = ax + e.x, sy = ay + e.y;
            if (sx >= dr.x && sx < dr.x + dr.w && sy >= dr.y && sy < dr.y + dr.h)
            {
                int idx = (sy - dr.y) / itemH_;
                if (idx >= 0 && idx < (int)menus_[openIdx_].items.size())
                {
                    auto& item = menus_[openIdx_].items[idx];
                    if (!item.separator && item.enabled)
                    {
                        if (item.onClick) item.onClick();
                        if (onItemSelected) onItemSelected(openIdx_, idx);
                    }
                }
                closeAll();
                return;
            }
        }

        //check title click
        int t = titleAtX(e.x);
        if (t >= 0)
        {
            if (t == openIdx_)
                closeAll();
            else
                openIdx_ = t;
            markDirty();
            return;
        }

        //click outside → close all
        closeAll();
    };

    onMouseUp = [this](MouseEvent&)
    {
        if (openIdx_ >= 0 && switching_)
        {
            switching_ = false;
        }
    };
}

int MenuBar::addMenu(const std::string& title)
{
    Menu m;
    m.title = title;
    menus_.push_back(std::move(m));
    markDirty();
    return (int)menus_.size() - 1;
}

int MenuBar::addItem(int menuIdx, const std::string& text,
                     std::function<void()> onClick,
                     const std::string& shortcut)
{
    if (menuIdx < 0 || menuIdx >= (int)menus_.size()) return -1;
    MenuItem mi;
    mi.text     = text;
    mi.shortcut = shortcut;
    mi.onClick  = std::move(onClick);
    menus_[menuIdx].items.push_back(std::move(mi));
    markDirty();
    return (int)menus_[menuIdx].items.size() - 1;
}

int MenuBar::addSeparator(int menuIdx)
{
    if (menuIdx < 0 || menuIdx >= (int)menus_.size()) return -1;
    MenuItem mi;
    mi.separator = true;
    menus_[menuIdx].items.push_back(std::move(mi));
    markDirty();
    return (int)menus_[menuIdx].items.size() - 1;
}

void MenuBar::clearMenu(int menuIdx)
{
    if (menuIdx >= 0 && menuIdx < (int)menus_.size())
    {
        menus_[menuIdx].items.clear();
        markDirty();
    }
}

void MenuBar::clearAll()
{
    menus_.clear();
    openIdx_ = -1;
    hoveredTitle_ = -1;
    hoveredItem_ = -1;
    markDirty();
}

void MenuBar::setBackgroundColor(Color color) { bgColor_ = color; markDirty(); }
void MenuBar::setTextColor(Color color) { textColor_ = color; markDirty(); }
void MenuBar::setHoverColor(Color color) { hoverColor_ = color; markDirty(); }
void MenuBar::setPopupColor(Color color) { popupColor_ = color; markDirty(); }
void MenuBar::setBorderColor(Color color) { borderColor_ = color; markDirty(); }
void MenuBar::setItemHoverColor(Color color) { itemHoverColor_ = color; markDirty(); }
void MenuBar::setDisabledTextColor(Color color) { disabledTextColor_ = color; markDirty(); }
void MenuBar::setShortcutColor(Color color) { shortcutColor_ = color; markDirty(); }
void MenuBar::setAccentColor(Color color) { accentColor_ = color; markDirty(); }

void MenuBar::closeAll()
{
    if (openIdx_ >= 0)
    {
        openIdx_ = -1;
        hoveredItem_ = -1;
        markDirty();
    }
}

int MenuBar::titleAtX(int localX) const
{
    int curX = 0;
    for (int i = 0; i < (int)menus_.size(); i++)
    {
        int tw = detail::textWidth(menus_[i].title.c_str());
        int pw = tw + titlePad_ * 2;
        if (localX >= curX && localX < curX + pw)
            return i;
        curX += pw;
    }
    return -1;
}

int MenuBar::dropdownLeft() const
{
    int curX = 0;
    for (int i = 0; i < openIdx_; i++)
    {
        int tw = detail::textWidth(menus_[i].title.c_str());
        curX += tw + titlePad_ * 2;
    }
    return curX;
}

int MenuBar::dropdownWidth() const
{
    int maxW = 0;
    for (const auto& mi : menus_[openIdx_].items)
    {
        if (mi.separator) continue;
        int tw = detail::textWidth(mi.text.c_str());
        int sw = mi.shortcut.empty() ? 0 : detail::textWidth(mi.shortcut.c_str()) + 20;
        maxW = std::max(maxW, tw + sw);
    }
    return std::max(maxW + 20, 120);
}

int MenuBar::dropdownHeight() const
{
    return (int)menus_[openIdx_].items.size() * itemH_;
}

Rect MenuBar::dropdownRect() const
{
    if (openIdx_ < 0) return { 0, 0, 0, 0 };
    int ax = absoluteX();
    int ay = absoluteY();
    return { ax + dropdownLeft(), ay + height_, dropdownWidth(), dropdownHeight() };
}

void MenuBar::switchTo_(int idx)
{
    openIdx_ = idx;
    hoveredItem_ = -1;
    switching_ = true;
    markDirty();
}

void MenuBar::onRender(SDL_Renderer* renderer)
{
    if (!visible_) return;

    Rect r = absoluteRect();

    //clip to menu bar area; children intersect with this clip
    detail::ClipGuard clip(renderer, r.x, r.y, r.w, r.h);

    //background
    SDL_SetRenderDrawColor(renderer, bgColor_.r, bgColor_.g, bgColor_.b, bgColor_.a);
    SDL_FRect fullBar = { (float)r.x, (float)r.y, (float)r.w, (float)r.h };
    SDL_RenderFillRect(renderer, &fullBar);

    //bottom border highlight for open menu title
    if (openIdx_ >= 0)
    {
        int ox = r.x + dropdownLeft();
        int ot = detail::textWidth(menus_[openIdx_].title.c_str());
        SDL_SetRenderDrawColor(renderer, accentColor_.r, accentColor_.g, accentColor_.b, accentColor_.a);
        SDL_RenderLine(renderer, ox, r.y + r.h - 1, ox + ot + titlePad_ * 2 - 1, r.y + r.h - 1);
    }

    //menu titles
    int curX = 0;
    for (int i = 0; i < (int)menus_.size(); i++)
    {
        int tw = detail::textWidth(menus_[i].title.c_str());
        int pw = tw + titlePad_ * 2;
        int tx = r.x + curX + titlePad_;
        int ty = r.y + (r.h - currentFont()->lineHeight()) / 2;

        bool highlight = (i == hoveredTitle_) || (i == openIdx_);
        if (highlight)
        {
            SDL_SetRenderDrawColor(renderer, hoverColor_.r, hoverColor_.g, hoverColor_.b, hoverColor_.a);
            SDL_FRect hr = { (float)(r.x + curX), (float)r.y, (float)pw, (float)r.h };
            SDL_RenderFillRect(renderer, &hr);
        }

        detail::drawText(renderer, menus_[i].title.c_str(), tx, ty, textColor_);
        curX += pw;
    }

    //dropdown uses renderDropdownOverlay (called after root->onRender)
    dirty_ = false;
}

void MenuBar::renderDropdownOverlay(SDL_Renderer* renderer)
{
    if (!visible_ || openIdx_ < 0) return;
    if (openIdx_ >= (int)menus_.size()) return;

    Rect dr = dropdownRect();

    //dropdown background
    SDL_SetRenderDrawColor(renderer, popupColor_.r, popupColor_.g, popupColor_.b, popupColor_.a);
    SDL_FRect dbg = { (float)dr.x, (float)dr.y, (float)dr.w, (float)dr.h };
    SDL_RenderFillRect(renderer, &dbg);

    //border
    SDL_SetRenderDrawColor(renderer, borderColor_.r, borderColor_.g, borderColor_.b, borderColor_.a);
    SDL_RenderRect(renderer, &dbg);

    //items
    auto& items = menus_[openIdx_].items;
    for (int i = 0; i < (int)items.size(); i++)
    {
        int ly = dr.y + i * itemH_;

        if (items[i].separator)
        {
            SDL_SetRenderDrawColor(renderer, borderColor_.r, borderColor_.g, borderColor_.b, borderColor_.a);
            SDL_RenderLine(renderer, dr.x + 4, ly + itemH_ / 2,
                               dr.x + dr.w - 4, ly + itemH_ / 2);
            continue;
        }

        //hover highlight
        if (i == hoveredItem_ && items[i].enabled)
        {
            SDL_SetRenderDrawColor(renderer, itemHoverColor_.r, itemHoverColor_.g,
                                   itemHoverColor_.b, itemHoverColor_.a);
            SDL_FRect hr = { (float)(dr.x + 1), (float)(ly + 1), (float)(dr.w - 2), (float)(itemH_ - 2) };
            SDL_RenderFillRect(renderer, &hr);
        }

        //text
        Color tc = items[i].enabled ? textColor_ : disabledTextColor_;
        detail::drawText(renderer, items[i].text.c_str(), dr.x + 6, ly + 3, tc);

        //shortcut
        if (!items[i].shortcut.empty() && items[i].enabled)
        {
            int sx = dr.x + dr.w - 6 - detail::textWidth(items[i].shortcut.c_str());
            detail::drawText(renderer, items[i].shortcut.c_str(), sx, ly + 3, shortcutColor_);
        }
    }
}

bool MenuBar::hitTest(int absX, int absY) const
{
    if (!Widget::hitTest(absX, absY))
    {
        //check dropdown area if open
        if (openIdx_ < 0) return false;
        Rect dr = dropdownRect();
        return absX >= dr.x && absX < dr.x + dr.w &&
               absY >= dr.y && absY < dr.y + dr.h;
    }
    return true;
}

//==============================================================================
// Box
//==============================================================================
Box::Box(Widget* parent, Orientation orientation)
    : Widget(parent), orientation_(orientation)
{
}

void Box::setOrientation(Orientation o)
{
    if (orientation_ == o) return;
    orientation_ = o;
    relayout();
}

void Box::setPadding(int padding)
{
    if (padding < 0) padding = 0;
    if (padding_ == padding) return;
    padding_ = padding;
    relayout();
}

void Box::setSpacing(int spacing)
{
    if (spacing < 0) spacing = 0;
    if (spacing_ == spacing) return;
    spacing_ = spacing;
    relayout();
}

void Box::layoutChildren()
{
    std::vector<Widget*> items;
    for (auto& child : children_)
        if (child->visible())
            items.push_back(child.get());

    int n = (int)items.size();
    if (n == 0) return;

    bool horiz = (orientation_ == Orientation::Horizontal);
    int contentW = std::max(0, width_ - padding_ * 2);
    int contentH = std::max(0, height_ - padding_ * 2);
    int mainSize = horiz ? contentW : contentH;
    int crossSize = horiz ? contentH : contentW;

    float totalStretch = 0.0f;
    int fixed = 0;
    int lastWeighted = -1;

    for (int i = 0; i < n; ++i)
    {
        Widget* c = items[i];
        if (c->layoutStretch_ > 0.0f)
        {
            totalStretch += c->layoutStretch_;
            lastWeighted = i;
        }
        else
        {
            fixed += horiz ? c->width_ : c->height_;
        }
    }

    int gaps = spacing_ * (n - 1);
    int remaining = std::max(0, mainSize - fixed - gaps);

    std::vector<int> sizes(n, 0);
    int used = 0;
    for (int i = 0; i < n; ++i)
    {
        Widget* c = items[i];
        if (c->layoutStretch_ > 0.0f && totalStretch > 0.0f)
        {
            if (i == lastWeighted)
            {
                sizes[i] = std::max(0, remaining - used);
            }
            else
            {
                sizes[i] = (int)(remaining * (c->layoutStretch_ / totalStretch));
                used += sizes[i];
            }
        }
        else
        {
            sizes[i] = horiz ? c->width_ : c->height_;
        }
    }

    int mainPos = padding_;
    for (int i = 0; i < n; ++i)
    {
        Widget* c = items[i];
        int childCross = c->fillCross_ ? crossSize
                                       : (horiz ? c->height_ : c->width_);

        if (horiz)
            c->setBoundsInternal(mainPos, padding_, sizes[i], childCross);
        else
            c->setBoundsInternal(padding_, mainPos, childCross, sizes[i]);

        mainPos += sizes[i] + spacing_;
    }
}

//==============================================================================
// SplitView
//==============================================================================
SplitView::SplitView(Widget* parent)
    : Widget(parent)
{
    onMouseDown = [this](MouseEvent& e)
    {
        if (e.button != SDL_BUTTON_LEFT) return;

        int p = (orientation_ == Orientation::SideBySide) ? e.x : e.y;
        int d = dividerPos();
        if (p < d) p = d - p; else p = p - d;
        if (p <= dividerW_ / 2 + 2)
            dragging_ = true;
    };

    onMouseMove = [this](MouseEvent& e)
    {
        if (!dragging_) return;
        splitFromMouse(e.x, e.y);
    };

    onMouseUp = [this](MouseEvent&)
    {
        dragging_ = false;
    };
}

void SplitView::setOrientation(Orientation o)
{
    if (orientation_ == o) return;
    orientation_ = o;
    relayout();
}

void SplitView::setSplit(float ratio)
{
    if (ratio < 0.0f) ratio = 0.0f;
    if (ratio > 1.0f) ratio = 1.0f;
    if (ratio_ == ratio) return;
    ratio_ = ratio;
    relayout();
}

void SplitView::setMinPaneSize(int px)
{
    if (px < 0) px = 0;
    if (minPane_ == px) return;
    minPane_ = px;
    relayout();
}

void SplitView::setDividerWidth(int px)
{
    if (px < 0) px = 0;
    if (dividerW_ == px) return;
    dividerW_ = px;
    relayout();
}

void SplitView::setDividerColor(Color color)
{
    dividerColor_ = color;
    markDirty();
}

int SplitView::dividerPos() const
{
    for (auto& child : children_)
    {
        if (!child->visible()) continue;
        return (orientation_ == Orientation::SideBySide) ? child->width()
                                                         : child->height();
    }
    return 0;
}

void SplitView::splitFromMouse(int localX, int localY)
{
    int main = (orientation_ == Orientation::SideBySide) ? width_ : height_;
    int avail = std::max(1, main - dividerW_);
    int p = (orientation_ == Orientation::SideBySide) ? localX : localY;
    p = std::max(0, std::min(p, avail));
    setSplit((float)p / (float)avail);
}

void SplitView::layoutChildren()
{
    std::vector<Widget*> panes;
    for (auto& child : children_)
    {
        if (!child->visible()) continue;
        panes.push_back(child.get());
        if (panes.size() == 2) break;
    }

    if (panes.empty()) return;

    bool side = (orientation_ == Orientation::SideBySide);
    int main = side ? width_ : height_;
    int cross = side ? height_ : width_;

    if (panes.size() == 1)
    {
        panes[0]->setBoundsInternal(0, 0, side ? main : cross, side ? cross : main);
        return;
    }

    int avail = std::max(0, main - dividerW_);
    int first = (int)(avail * ratio_ + 0.5f);

    if (minPane_ > 0 && avail >= minPane_ * 2)
        first = std::max(minPane_, std::min(first, avail - minPane_));
    else
        first = std::max(0, std::min(first, avail));

    int second = avail - first;

    if (side)
    {
        panes[0]->setBoundsInternal(0, 0, first, cross);
        panes[1]->setBoundsInternal(first + dividerW_, 0, second, cross);
    }
    else
    {
        panes[0]->setBoundsInternal(0, 0, cross, first);
        panes[1]->setBoundsInternal(0, first + dividerW_, cross, second);
    }
}

void SplitView::onRender(SDL_Renderer* renderer)
{
    Widget::onRender(renderer);
    if (!visible_) return;

    int visiblePanes = 0;
    for (auto& child : children_)
        if (child->visible()) ++visiblePanes;
    if (visiblePanes < 2) return;

    Rect r = absoluteRect();
    detail::ClipGuard clip(renderer, r.x, r.y, r.w, r.h);

    int d = dividerPos();
    SDL_FRect dr;
    if (orientation_ == Orientation::SideBySide)
        dr = { (float)(r.x + d), (float)r.y, (float)dividerW_, (float)r.h };
    else
        dr = { (float)r.x, (float)(r.y + d), (float)r.w, (float)dividerW_ };

    SDL_SetRenderDrawColor(renderer,
        dividerColor_.r, dividerColor_.g, dividerColor_.b, dividerColor_.a);
    SDL_RenderFillRect(renderer, &dr);
}

//==============================================================================
// Canvas
//==============================================================================
void Canvas::onRender(SDL_Renderer* renderer)
{
    if (!visible_) return;

    if (onDraw)
    {
        Rect r = absoluteRect();
        detail::ClipGuard clip(renderer, r.x, r.y, r.w, r.h);
        onDraw(renderer, r);
    }

    for (auto& child : children_)
        child->onRender(renderer);

    dirty_ = false;
}

//==============================================================================
// Image
//==============================================================================
#ifdef SDLGUI_USE_SDL_IMAGE
Image::~Image()
{
    if (texture_) SDL_DestroyTexture(texture_);
    if (surfaceCache_) SDL_DestroySurface(surfaceCache_);
}

bool Image::load(const char* path)
{
    SDL_Surface* surf = IMG_Load(path);
    if (!surf) return false;
    texW_ = surf->w;
    texH_ = surf->h;
    if (texture_)
    {
        SDL_DestroyTexture(texture_);
        texture_ = nullptr;
    }
    if (surfaceCache_)
        SDL_DestroySurface(surfaceCache_);
    surfaceCache_ = surf;
    markDirty();
    return true;
}

void Image::setTexture(SDL_Texture* tex)
{
    if (texture_) SDL_DestroyTexture(texture_);
    texture_ = tex;
    if (tex)
    {
        float tw = 0.f, th = 0.f;
        if (SDL_GetTextureSize(tex, &tw, &th))
        {
            texW_ = (int)tw;
            texH_ = (int)th;
        }
    }
    if (surfaceCache_)
    {
        SDL_DestroySurface(surfaceCache_);
        surfaceCache_ = nullptr;
    }
    markDirty();
}

void Image::setColor(Color color)
{
    modColor_ = color;
    markDirty();
}

void Image::onRender(SDL_Renderer* renderer)
{
    if (!visible_) return;

    Rect r = absoluteRect();
    detail::ClipGuard clip(renderer, r.x, r.y, r.w, r.h);

    if (surfaceCache_ && !texture_)
    {
        texture_ = SDL_CreateTextureFromSurface(renderer, surfaceCache_);
        if (texture_)
        {
            SDL_DestroySurface(surfaceCache_);
            surfaceCache_ = nullptr;
        }
    }

    if (texture_)
    {
        SDL_SetTextureColorMod(texture_, modColor_.r, modColor_.g, modColor_.b);
        SDL_SetTextureAlphaMod(texture_, modColor_.a);
        SDL_FRect dst = { (float)r.x, (float)r.y, (float)r.w, (float)r.h };
        SDL_RenderTexture(renderer, texture_, nullptr, &dst);
    }

    for (auto& child : children_)
        child->onRender(renderer);

    dirty_ = false;
}
#endif //SDLGUI_USE_SDL_IMAGE

//==============================================================================
// Label
//==============================================================================
void Label::setText(const std::string& text)
{
    text_ = text;
    layoutWidth_ = -1;
    markDirty();
}

void Label::setTextColor(Color color)
{
    textColor_ = color;
    markDirty();
}

void Label::setWordWrap(bool wrap)
{
    wordWrap_ = wrap;
    layoutWidth_ = -1;
    layoutFontEpoch_ = 0;
    markDirty();
}

void Label::onFontChanged()
{
    layoutWidth_ = -1;
    layoutFontEpoch_ = 0;
    markDirty();
}

void Label::onRender(SDL_Renderer* renderer)
{
    if (!visible_) return;

    Rect r = absoluteRect();
    detail::ClipGuard clip(renderer, r.x, r.y, r.w, r.h);

    //Layout is cached and only recomputed when text, wrap mode, width or font changes.
    if (layoutWidth_ != r.w || layoutWrap_ != wordWrap_ ||
        layoutFontEpoch_ != fontEpoch())
    {
        detail::wrapText(text_.c_str(),
                         wordWrap_ ? r.w : 0x7fffffff, layout_);
        layoutWidth_ = r.w;
        layoutWrap_  = wordWrap_;
        layoutFontEpoch_ = fontEpoch();
    }

    int lineH = currentFont()->lineHeight();
    int curY = r.y;
    for (const auto& line : layout_)
    {
        if (!line.empty())
            detail::drawText(renderer, line.c_str(), r.x, curY, textColor_);
        curY += lineH;
    }

    for (auto& child : children_)
        child->onRender(renderer);

    dirty_ = false;
}

//==============================================================================
// Button
//==============================================================================
Button::Button(Widget* parent)
    : Widget(parent)
{
    focusable_ = true;
    onMouseEnter = [this](MouseEvent&) { state_ = ButtonState::Hover;   markDirty(); };
    onMouseLeave = [this](MouseEvent&) { state_ = ButtonState::Normal;  markDirty(); };
    onMouseDown  = [this](MouseEvent&) { state_ = ButtonState::Pressed; markDirty(); };
    onMouseUp    = [this](MouseEvent&) { state_ = ButtonState::Hover;   markDirty(); };
}

void Button::setText(const std::string& text)
{
    text_ = text;
    markDirty();
}

void Button::setTextColor(Color color)
{
    textColor_ = color;
    markDirty();
}

void Button::setBgColor(Color normal, Color hover, Color pressed)
{
    bgNormal_  = normal;
    bgHover_   = hover;
    bgPressed_ = pressed;
    markDirty();
}

void Button::setBorderColor(Color color)
{
    borderColor_ = color;
    markDirty();
}

void Button::onRender(SDL_Renderer* renderer)
{
    if (!visible_) return;

    Rect r = absoluteRect();
    SDL_FRect fc   = { (float)r.x, (float)r.y, (float)r.w, (float)r.h };
    detail::ClipGuard clip(renderer, r.x, r.y, r.w, r.h);

    //background
    Color bg;
    switch (state_)
    {
    case ButtonState::Hover:   bg = bgHover_;   break;
    case ButtonState::Pressed: bg = bgPressed_; break;
    default:                   bg = bgNormal_;  break;
    }
    if (!enabled_)
    {
        bg.r = bg.r * 2 / 3;
        bg.g = bg.g * 2 / 3;
        bg.b = bg.b * 2 / 3;
    }
    SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
    SDL_RenderFillRect(renderer, &fc);

    //border
    Color bc = enabled_ ? borderColor_ : Color(borderColor_.r * 2 / 3,
                                                borderColor_.g * 2 / 3,
                                                borderColor_.b * 2 / 3);
    SDL_SetRenderDrawColor(renderer, bc.r, bc.g, bc.b, bc.a);
    SDL_RenderRect(renderer, &fc);

    //text
    Color tc = enabled_ ? textColor_ : Color(textColor_.r * 2 / 3, textColor_.g * 2 / 3, textColor_.b * 2 / 3);
    int tx = r.x + (r.w - detail::textWidth(text_.c_str())) / 2;
    int ty = r.y + (r.h - currentFont()->lineHeight()) / 2;
    detail::drawText(renderer, text_.c_str(), tx, ty, tc);

    for (auto& child : children_)
        child->onRender(renderer);

    dirty_ = false;
}

//==============================================================================
// CheckBox
//==============================================================================
CheckBox::CheckBox(Widget* parent)
    : Widget(parent)
{
    focusable_ = true;
    onClick = [this](MouseEvent&)
    {
        setChecked(!checked_);
    };
}

void CheckBox::setText(const std::string& text)
{
    text_ = text;
    markDirty();
}

void CheckBox::setChecked(bool checked)
{
    checked_ = checked;
    markDirty();
}

void CheckBox::setTextColor(Color color)
{
    textColor_ = color;
    markDirty();
}

void CheckBox::setCheckColor(Color color)
{
    checkColor_ = color;
    markDirty();
}

void CheckBox::setBoxColor(Color color)
{
    boxColor_ = color;
    markDirty();
}

void CheckBox::setBorderColor(Color color)
{
    borderColor_ = color;
    markDirty();
}

void CheckBox::onRender(SDL_Renderer* renderer)
{
    if (!visible_) return;

    Rect r = absoluteRect();
    detail::ClipGuard clip(renderer, r.x, r.y, r.w, r.h);

    //box
    Color boxC = enabled_ ? boxColor_ : Color(boxColor_.r * 2 / 3,
                                               boxColor_.g * 2 / 3,
                                               boxColor_.b * 2 / 3);
    SDL_FRect box = { (float)r.x, (float)(r.y + (r.h - 16) / 2), 16.0f, 16.0f };
    SDL_SetRenderDrawColor(renderer, boxC.r, boxC.g, boxC.b, boxC.a);
    SDL_RenderFillRect(renderer, &box);
    Color bc = enabled_ ? borderColor_ : Color(borderColor_.r * 2 / 3,
                                                borderColor_.g * 2 / 3,
                                                borderColor_.b * 2 / 3);
    SDL_SetRenderDrawColor(renderer, bc.r, bc.g, bc.b, bc.a);
    SDL_RenderRect(renderer, &box);

    //check mark
    if (checked_)
    {
        Color cc = enabled_ ? checkColor_ : Color(checkColor_.r * 2 / 3, checkColor_.g * 2 / 3, checkColor_.b * 2 / 3);
        SDL_SetRenderDrawColor(renderer, cc.r, cc.g, cc.b, cc.a);
        //simple X check
        SDL_RenderLine(renderer, box.x + 3, box.y + 8, box.x + 7, box.y + 12);
        SDL_RenderLine(renderer, box.x + 7, box.y + 12, box.x + 13, box.y + 4);
        SDL_RenderLine(renderer, box.x + 3, box.y + 9, box.x + 7, box.y + 13);
        SDL_RenderLine(renderer, box.x + 7, box.y + 13, box.x + 13, box.y + 5);
    }

    //text
    Color tc = enabled_ ? textColor_ : Color(textColor_.r * 2 / 3, textColor_.g * 2 / 3, textColor_.b * 2 / 3);
    detail::drawText(renderer, text_.c_str(), r.x + 22,
                     r.y + (r.h - currentFont()->lineHeight()) / 2, tc);

    for (auto& child : children_)
        child->onRender(renderer);

    dirty_ = false;
}

//==============================================================================
// TextBox
//==============================================================================
TextBox::TextBox(Widget* parent)
    : Widget(parent)
{
    focusable_ = true;

    onMouseDown = [this](MouseEvent& e)
    {
        if (e.button != SDL_BUTTON_LEFT) return;

        int pos = positionAtX(e.x);

        if (SDL_GetModState() & SDL_KMOD_SHIFT)
        {
            if (selAnchor_ < 0) selAnchor_ = cursorPos_;
            cursorPos_ = pos;
        }
        else
        {
            selAnchor_ = pos;
            cursorPos_ = pos;
        }

        mouseSelecting_ = true;
        adjustScroll();
        markDirty();
    };

    onMouseMove = [this](MouseEvent& e)
    {
        if (!mouseSelecting_) return;
        cursorPos_ = positionAtX(e.x);
        adjustScroll();
        markDirty();
    };

    onMouseUp = [this](MouseEvent&)
    {
        mouseSelecting_ = false;
    };

    onFocus = [this]() { markDirty(); };
    onBlur = [this]()
    {
        mouseSelecting_ = false;
        selAnchor_ = -1;
        markDirty();
    };
}

int TextBox::positionAtX(int localX) const
{
    int target = localX - 4 - scrollOffX_;
    int pos = 0;
    int x = 0;
    int maskW = password_ ? maskCharWidth() : 0;
    const char* p = text_.c_str();

    while (*p)
    {
        const char* prev = p;
        uint32_t cp = detail::decodeUTF8(p);
        int bytes = (int)(p - prev);
        int cw = password_ ? maskW : detail::glyphWidth(cp);

        if (x + cw > target)
            break;

        x += cw;
        pos += bytes;
    }

    return pos;
}

void TextBox::setText(const std::string& text)
{
    text_ = text;
    text_.erase(std::remove(text_.begin(), text_.end(), '\n'), text_.end());
    text_.erase(std::remove(text_.begin(), text_.end(), '\r'), text_.end());
    truncateToMaxLength();
    cursorPos_ = (int)text_.size();
    selAnchor_ = -1;
    mouseSelecting_ = false;
    adjustScroll();
    markDirty();
}

void TextBox::setTextColor(Color color)
{
    textColor_ = color;
    markDirty();
}

void TextBox::setBackgroundColor(Color color)
{
    bgColor_ = color;
    markDirty();
}

void TextBox::setCursorColor(Color color)
{
    cursorColor_ = color;
    markDirty();
}

void TextBox::setSelectionColor(Color color)
{
    selColor_ = color;
    markDirty();
}

void TextBox::setBorderColor(Color color)
{
    borderColor_ = color;
    markDirty();
}

void TextBox::setPlaceholder(const std::string& text)
{
    placeholder_ = text;
    markDirty();
}

void TextBox::setPlaceholderColor(Color color)
{
    placeholderColor_ = color;
    markDirty();
}

void TextBox::setMaxLength(int maxChars)
{
    maxLength_ = std::max(0, maxChars);
    truncateToMaxLength();
    adjustScroll();
    markDirty();
}

void TextBox::setPassword(bool on)
{
    if (password_ == on) return;
    password_ = on;
    adjustScroll();
    markDirty();
}

void TextBox::setPasswordChar(char c)
{
    passwordChar_ = c ? c : '*';
    if (password_)
    {
        adjustScroll();
        markDirty();
    }
}

int TextBox::codepointCount(const std::string& text)
{
    int n = 0;
    const char* p = text.c_str();
    while (*p)
    {
        detail::decodeUTF8(p);
        ++n;
    }
    return n;
}

void TextBox::truncateToMaxLength()
{
    if (maxLength_ <= 0) return;

    const char* s = text_.c_str();
    const char* p = s;
    int n = 0;
    while (*p && n < maxLength_)
    {
        detail::decodeUTF8(p);
        ++n;
    }
    if (!*p) return;

    text_.resize((size_t)(p - s));
    if (cursorPos_ > (int)text_.size())
        cursorPos_ = (int)text_.size();
    selAnchor_ = -1;
}

void TextBox::insertText(const std::string& text)
{
    std::string ins = text;
    ins.erase(std::remove(ins.begin(), ins.end(), '\n'), ins.end());
    ins.erase(std::remove(ins.begin(), ins.end(), '\r'), ins.end());
    if (ins.empty()) return;

    if (hasSelection())
        deleteSelection();

    if (maxLength_ > 0)
    {
        int room = maxLength_ - codepointCount(text_);
        if (room <= 0) return;

        int n = 0;
        const char* p = ins.c_str();
        while (*p && n < room)
        {
            detail::decodeUTF8(p);
            ++n;
        }
        if (*p)
            ins.resize((size_t)(p - ins.c_str()));
    }

    text_.insert(cursorPos_, ins);
    cursorPos_ += (int)ins.size();
    selAnchor_ = -1;
    adjustScroll();
    markDirty();
}

int TextBox::maskCharWidth() const
{
    return detail::glyphWidth((uint32_t)(unsigned char)passwordChar_);
}

int TextBox::displayWidthTo(int bytePos) const
{
    int w = 0;
    int consumed = 0;
    int maskW = password_ ? maskCharWidth() : 0;
    const char* p = text_.c_str();

    while (*p && consumed < bytePos)
    {
        const char* prev = p;
        uint32_t cp = detail::decodeUTF8(p);
        int bytes = (int)(p - prev);
        if (consumed + bytes > bytePos) break;

        consumed += bytes;
        w += password_ ? maskW : detail::glyphWidth(cp);
    }
    return w;
}

int TextBox::displayWidth() const
{
    return displayWidthTo((int)text_.size());
}

std::string TextBox::displayText() const
{
    if (!password_) return text_;

    std::string out;
    out.reserve(text_.size());
    const char* p = text_.c_str();
    while (*p)
    {
        detail::decodeUTF8(p);
        out.push_back(passwordChar_);
    }
    return out;
}

bool TextBox::scrollByPixels(int dx)
{
    if (dx == 0) return false;

    int textW = displayWidth();
    int visibleW = width_ - 8;
    if (textW <= visibleW)
    {
        if (scrollOffX_ != 0)
        {
            scrollOffX_ = 0;
            markDirty();
            return true;
        }
        return false;
    }

    int minScroll = visibleW - textW;
    int next = scrollOffX_ + dx;
    if (next > 0) next = 0;
    if (next < minScroll) next = minScroll;

    if (next != scrollOffX_)
    {
        scrollOffX_ = next;
        markDirty();
        return true;
    }
    return false;
}

void TextBox::selectAll()
{
    selAnchor_ = 0;
    cursorPos_ = (int)text_.size();
    adjustScroll();
    markDirty();
}

void TextBox::clearSelection()
{
    if (selAnchor_ >= 0)
    {
        selAnchor_ = -1;
        markDirty();
    }
}

std::string TextBox::selectedText() const
{
    if (!hasSelection()) return {};

    int start = std::min(selAnchor_, cursorPos_);
    int end   = std::max(selAnchor_, cursorPos_);
    return text_.substr(start, end - start);
}

bool TextBox::deleteSelection()
{
    if (!hasSelection()) return false;

    int start = std::min(selAnchor_, cursorPos_);
    int end   = std::max(selAnchor_, cursorPos_);
    text_.erase(start, end - start);
    cursorPos_ = start;
    selAnchor_ = -1;
    adjustScroll();
    markDirty();
    return true;
}

void TextBox::adjustScroll()
{
    int cw       = displayWidthTo(cursorPos_);
    int textW    = displayWidth();
    int visibleW = width_ - 8;

    if (visibleW <= 0)
    {
        scrollOffX_ = 0;
        return;
    }

    if (textW <= visibleW)
    {
        scrollOffX_ = 0;
        return;
    }

    int cursorX = scrollOffX_ + cw;

    //cursor beyond right edge → scroll left
    if (cursorX + 8 > visibleW)
        scrollOffX_ = visibleW - cw - 8;

    //cursor beyond left edge → scroll right
    if (scrollOffX_ + cw < 0)
        scrollOffX_ = -cw;

    //clamp
    if (scrollOffX_ > 0) scrollOffX_ = 0;
    int minScroll = visibleW - textW;
    if (scrollOffX_ < minScroll) scrollOffX_ = minScroll;
}

bool TextBox::textInputRect(Rect& rect) const
{
    Rect r = absoluteRect();
    int lineH = currentFont()->lineHeight();
    int cx = r.x + 4 + scrollOffX_ + displayWidthTo(cursorPos_);
    int cy = r.y + (r.h - lineH) / 2;
    rect = { cx, cy, 1, lineH };
    return true;
}

void TextBox::onFontChanged()
{
    adjustScroll();
    markDirty();
}

void TextBox::onRender(SDL_Renderer* renderer)
{
    if (!visible_) return;

    Rect r = absoluteRect();
    SDL_FRect fc  = { (float)r.x, (float)r.y, (float)r.w, (float)r.h };
    detail::ClipGuard clip(renderer, r.x, r.y, r.w, r.h);

    //background
    SDL_SetRenderDrawColor(renderer, bgColor_.r, bgColor_.g, bgColor_.b, bgColor_.a);
    SDL_RenderFillRect(renderer, &fc);

    //border (no special focus color; the caret indicates focus)
    SDL_SetRenderDrawColor(renderer, borderColor_.r, borderColor_.g,
                           borderColor_.b, borderColor_.a);
    SDL_RenderRect(renderer, &fc);

    int lineH = currentFont()->lineHeight();
    int tx = r.x + 4 + scrollOffX_;
    int ty = r.y + (r.h - lineH) / 2;

    //selection background
    if (hasSelection())
    {
        int s = std::min(selAnchor_, cursorPos_);
        int e = std::max(selAnchor_, cursorPos_);
        int sx = tx + displayWidthTo(s);
        int ex = tx + displayWidthTo(e);

        if (ex > sx)
        {
            SDL_FRect sel = { (float)sx, (float)ty, (float)(ex - sx), (float)lineH };
            SDL_SetRenderDrawColor(renderer, selColor_.r, selColor_.g, selColor_.b, selColor_.a);
            SDL_RenderFillRect(renderer, &sel);
        }
    }

    //text or placeholder (with scroll offset)
    if (text_.empty() && !placeholder_.empty())
    {
        detail::drawText(renderer, placeholder_.c_str(), tx, ty, placeholderColor_);
    }
    else
    {
        std::string shown = displayText();
        detail::drawText(renderer, shown.c_str(), tx, ty, textColor_);
    }

    //cursor
    if (hasFocus_)
    {
        uint32_t elapsed = SDL_GetTicks() - focusTick_;
        if ((elapsed / 500) % 2 == 0) //blink 500ms
        {
            int cx = tx + displayWidthTo(cursorPos_);
            SDL_SetRenderDrawColor(renderer,
                cursorColor_.r, cursorColor_.g, cursorColor_.b, cursorColor_.a);
            SDL_RenderLine(renderer, cx, ty, cx, ty + lineH);
        }
    }

    for (auto& child : children_)
        child->onRender(renderer);

    dirty_ = false;
}

//==============================================================================
// ListBox
//==============================================================================
ListBox::ListBox(Widget* parent)
    : Widget(parent)
{
    focusable_ = true;
    itemH_ = 20;
    onMouseDown = [this](MouseEvent& e)
    {
        int contentH = (int)items_.size() * itemH_;
        int sbW = 10;

        //scrollbar
        if (contentH > height_ && e.x >= width_ - sbW)
        {
            int maxOff = contentH - height_;
            int thumbH = std::max(20, height_ * height_ / contentH);
            int trackH = height_ - thumbH;
            int thumbY = trackH > 0
                ? (int)((long long)trackH * scrollOffY_ / std::max(1, maxOff))
                : 0;

            if (e.y >= thumbY && e.y < thumbY + thumbH)
            {
                draggingScrollbar_ = true;
                dragStartY_ = e.y;
                dragStartOff_ = scrollOffY_;
            }
            else
            {
                int page = std::max(itemH_, height_ - itemH_);
                if (e.y < thumbY) scrollOffY_ -= page;
                else              scrollOffY_ += page;
                if (scrollOffY_ < 0) scrollOffY_ = 0;
                if (scrollOffY_ > maxOff) scrollOffY_ = maxOff;
                markDirty();
            }
            return;
        }

        int idx = (e.y + scrollOffY_) / itemH_;
        if (idx >= 0 && idx < (int)items_.size())
        {
            setSelectedIndex(idx);
            if (onItemSelected)
                onItemSelected(idx);
        }
    };
    onMouseMove = [this](MouseEvent& e)
    {
        if (draggingScrollbar_)
        {
            int contentH = (int)items_.size() * itemH_;
            int maxOff = std::max(0, contentH - height_);
            int thumbH = std::max(20, height_ * height_ / std::max(1, contentH));
            int trackH = height_ - thumbH;
            if (trackH > 0 && maxOff > 0)
            {
                int off = dragStartOff_ +
                    (int)((long long)(e.y - dragStartY_) * maxOff / trackH);
                if (off < 0) off = 0;
                if (off > maxOff) off = maxOff;
                if (off != scrollOffY_)
                {
                    scrollOffY_ = off;
                    markDirty();
                }
            }
            return;
        }

        int idx = (e.y + scrollOffY_) / itemH_;
        if (e.x >= width_ - 10 && (int)items_.size() * itemH_ > height_)
            idx = -1;

        int old = hovered_;
        hovered_ = (idx >= 0 && idx < (int)items_.size()) ? idx : -1;
        if (old != hovered_) markDirty();
    };
    onMouseUp = [this](MouseEvent&)
    {
        draggingScrollbar_ = false;
    };
    onMouseLeave = [this](MouseEvent&)
    {
        if (hovered_ != -1) { hovered_ = -1; markDirty(); }
    };
}

void ListBox::addItem(const std::string& item)
{
    items_.push_back(item);
    markDirty();
}

void ListBox::removeItem(int index)
{
    if (index >= 0 && index < (int)items_.size())
    {
        items_.erase(items_.begin() + index);
        if (selected_ == index) selected_ = -1;
        markDirty();
    }
}

void ListBox::clear()
{
    items_.clear();
    selected_ = -1;
    markDirty();
}

void ListBox::onMouseWheel(int delta)
{
    scrollOffY_ -= delta * 20;
    int maxOff = (int)items_.size() * itemH_ - height_;
    if (scrollOffY_ < 0) scrollOffY_ = 0;
    if (scrollOffY_ > maxOff) scrollOffY_ = std::max(0, maxOff);
    markDirty();
}

void ListBox::setBackgroundColor(Color color) { bgColor_ = color; markDirty(); }
void ListBox::setTextColor(Color color) { textColor_ = color; markDirty(); }
void ListBox::setSelectionColor(Color color) { selColor_ = color; markDirty(); }
void ListBox::setHoverColor(Color color) { hoverColor_ = color; markDirty(); }
void ListBox::setBorderColor(Color color) { borderColor_ = color; markDirty(); }

void ListBox::setSelectedIndex(int index)
{
    if (index >= -1 && index < (int)items_.size())
    {
        selected_ = index;
        markDirty();
    }
}

const std::string& ListBox::selectedItem() const
{
    static std::string empty;
    if (selected_ >= 0 && selected_ < (int)items_.size())
        return items_[selected_];
    return empty;
}

void ListBox::onRender(SDL_Renderer* renderer)
{
    if (!visible_) return;

    Rect r = absoluteRect();
    detail::ClipGuard clip(renderer, r.x, r.y, r.w, r.h);

    SDL_FRect bg = { (float)r.x, (float)r.y, (float)r.w, (float)r.h };
    SDL_SetRenderDrawColor(renderer, bgColor_.r, bgColor_.g, bgColor_.b, bgColor_.a);
    SDL_RenderFillRect(renderer, &bg);

    for (int i = 0; i < (int)items_.size(); i++)
    {
        int y = r.y + i * itemH_ - scrollOffY_;
        if (y + itemH_ < r.y || y > r.y + r.h) continue;  //skip offscreen

        SDL_FRect row = { (float)r.x, (float)y, (float)r.w, (float)itemH_ };

        if (i == selected_)
        {
            SDL_SetRenderDrawColor(renderer, selColor_.r, selColor_.g, selColor_.b, selColor_.a);
            SDL_RenderFillRect(renderer, &row);
            detail::drawText(renderer, items_[i].c_str(), r.x + 4, y + 2, textColor_);
        }
        else if (i == hovered_)
        {
            SDL_SetRenderDrawColor(renderer, hoverColor_.r, hoverColor_.g, hoverColor_.b, hoverColor_.a);
            SDL_RenderFillRect(renderer, &row);
            detail::drawText(renderer, items_[i].c_str(), r.x + 4, y + 2, textColor_);
        }
        else
        {
            detail::drawText(renderer, items_[i].c_str(), r.x + 4, y + 2, textColor_);
        }
    }

    int contentH = (int)items_.size() * itemH_;
    if (contentH > height_)
    {
        const int sbW = 10;
        int maxOff = contentH - height_;
        int thumbH = std::max(20, height_ * height_ / contentH);
        int trackH = height_ - thumbH;
        int thumbY = trackH > 0
            ? (int)((long long)trackH * scrollOffY_ / std::max(1, maxOff))
            : 0;

        SDL_FRect track = { (float)(r.x + r.w - sbW), (float)r.y,
                            (float)sbW, (float)r.h };
        SDL_SetRenderDrawColor(renderer,
            (Uint8)(bgColor_.r * 9 / 10),
            (Uint8)(bgColor_.g * 9 / 10),
            (Uint8)(bgColor_.b * 9 / 10), 255);
        SDL_RenderFillRect(renderer, &track);

        SDL_FRect thumb = { (float)(r.x + r.w - sbW + 1),
                            (float)(r.y + thumbY),
                            (float)(sbW - 2), (float)thumbH };
        SDL_SetRenderDrawColor(renderer, borderColor_.r, borderColor_.g,
                               borderColor_.b, borderColor_.a);
        SDL_RenderFillRect(renderer, &thumb);
        SDL_SetRenderDrawColor(renderer,
            (Uint8)((borderColor_.r + textColor_.r) / 2),
            (Uint8)((borderColor_.g + textColor_.g) / 2),
            (Uint8)((borderColor_.b + textColor_.b) / 2), 255);
        SDL_RenderRect(renderer, &thumb);
    }

    SDL_SetRenderDrawColor(renderer, borderColor_.r, borderColor_.g, borderColor_.b, borderColor_.a);
    SDL_RenderRect(renderer, &bg);

    for (auto& child : children_)
        child->onRender(renderer);

    dirty_ = false;
}

//==============================================================================
// ContextMenu
//==============================================================================
ContextMenu::ContextMenu(Widget* parent)
    : Widget(parent)
{
    width_ = 160;
    onMouseMove = [this](MouseEvent& e)
    {
        int old = hoveredIdx_;
        hoveredIdx_ = e.y / 22;
        if (hoveredIdx_ < 0 || hoveredIdx_ >= (int)items_.size())
            hoveredIdx_ = -1;
        if (old != hoveredIdx_) markDirty();
    };
    onMouseLeave = [this](MouseEvent&)
    {
        if (hoveredIdx_ != -1) { hoveredIdx_ = -1; markDirty(); }
    };
}

void ContextMenu::addItem(const std::string& text, std::function<void()> onClick)
{
    ContextMenuItem item;
    item.text = text;
    item.onClick = std::move(onClick);
    items_.push_back(std::move(item));
    int w = detail::textWidth(text.c_str()) + 16;
    if (w > width_) width_ = w;
    markDirty();
}

void ContextMenu::addSeparator()
{
    ContextMenuItem item;
    item.separator = true;
    items_.push_back(std::move(item));
}

void ContextMenu::clear()
{
    items_.clear();
    hoveredIdx_ = -1;
    width_ = 160;
    markDirty();
}

void ContextMenu::setBackgroundColor(Color color) { bgColor_ = color; markDirty(); }
void ContextMenu::setTextColor(Color color) { textColor_ = color; markDirty(); }
void ContextMenu::setHoverColor(Color color) { hoverColor_ = color; markDirty(); }
void ContextMenu::setBorderColor(Color color) { borderColor_ = color; markDirty(); }
void ContextMenu::setDisabledTextColor(Color color) { disabledTextColor_ = color; markDirty(); }
void ContextMenu::setSeparatorColor(Color color) { separatorColor_ = color; markDirty(); }

void ContextMenu::show(int screenX, int screenY)
{
    screenX_ = screenX;
    screenY_ = screenY;
    active_ = true;
    setPosition(screenX, screenY);
    setSize(width_, (int)items_.size() * 22);
    markDirty();
}

void ContextMenu::dismiss()
{
    if (!active_) return;
    active_ = false;
    if (onDismiss) onDismiss();
    markDirty();
}

void ContextMenu::onRender(SDL_Renderer* renderer)
{
    if (!active_ || !visible_) return;

    Rect r = absoluteRect();
    SDL_FRect fc  = { (float)r.x, (float)r.y, (float)r.w, (float)r.h };
    detail::ClipGuard clip(renderer, r.x, r.y, r.w, r.h);

    //background
    SDL_SetRenderDrawColor(renderer, bgColor_.r, bgColor_.g, bgColor_.b, bgColor_.a);
    SDL_RenderFillRect(renderer, &fc);

    //border
    SDL_SetRenderDrawColor(renderer, borderColor_.r, borderColor_.g, borderColor_.b, borderColor_.a);
    SDL_RenderRect(renderer, &fc);

    for (int i = 0; i < (int)items_.size(); i++)
    {
        int iy = r.y + i * 22;

        if (items_[i].separator)
        {
            SDL_SetRenderDrawColor(renderer, separatorColor_.r, separatorColor_.g,
                                   separatorColor_.b, separatorColor_.a);
            SDL_RenderLine(renderer, r.x + 4, iy + 11, r.x + r.w - 4, iy + 11);
            continue;
        }

        if (i == hoveredIdx_ && items_[i].enabled)
        {
            SDL_SetRenderDrawColor(renderer, hoverColor_.r, hoverColor_.g,
                                   hoverColor_.b, hoverColor_.a);
            SDL_FRect sel = { (float)r.x, (float)iy, (float)r.w, 22.0f };
            SDL_RenderFillRect(renderer, &sel);
        }

        Color tc = items_[i].enabled ? textColor_ : disabledTextColor_;
        detail::drawText(renderer, items_[i].text.c_str(), r.x + 8, iy + 3, tc);
    }

    dirty_ = false;
}

bool ContextMenu::hitTest(int absX, int absY) const
{
    if (!active_) return false;
    return Widget::hitTest(absX, absY);
}

//==============================================================================
// Slider
//==============================================================================
Slider::Slider(Widget* parent)
    : Widget(parent)
{
    focusable_ = true;
    height_ = 24;
    onMouseDown = [this](MouseEvent& e)
    {
        dragging_ = true;
        if (onDragChanged) onDragChanged(true);
        float range = max_ - min_;
        int thumbPos = width_ - 16;
        float ratio = (float)(e.x - 8) / std::max(1, thumbPos);
        value_ = min_ + ratio * range;
        if (value_ < min_) value_ = min_;
        if (value_ > max_) value_ = max_;
        if (onValueChanged) onValueChanged(value_);
        markDirty();
    };
    onMouseMove = [this](MouseEvent& e)
    {
        if (!dragging_) return;
        float range = max_ - min_;
        int thumbPos = width_ - 16;
        float ratio = (float)(e.x - 8) / std::max(1, thumbPos);
        value_ = min_ + ratio * range;
        if (value_ < min_) value_ = min_;
        if (value_ > max_) value_ = max_;
        if (onValueChanged) onValueChanged(value_);
        markDirty();
    };
    onMouseUp = [this](MouseEvent&)
    {
        dragging_ = false;
        if (onDragChanged) onDragChanged(false);
    };
}

void Slider::setRange(float min, float max)
{
    min_ = min;
    max_ = max;
    if (value_ < min_) value_ = min_;
    if (value_ > max_) value_ = max_;
    markDirty();
}

void Slider::setValue(float val)
{
    value_ = val;
    if (value_ < min_) value_ = min_;
    if (value_ > max_) value_ = max_;
    markDirty();
}

void Slider::setTrackColor(Color color) { trackColor_ = color; markDirty(); }
void Slider::setFillColor(Color color) { fillColor_ = color; markDirty(); }
void Slider::setThumbColor(Color normal, Color hover)
{
    thumbColor_ = normal;
    thumbHoverColor_ = hover;
    markDirty();
}
void Slider::setBorderColor(Color color) { borderColor_ = color; markDirty(); }

void Slider::onRender(SDL_Renderer* renderer)
{
    if (!visible_) return;

    Rect r = absoluteRect();
    detail::ClipGuard clip(renderer, r.x, r.y, r.w, r.h);

    //track
    int trackY = r.y + r.h / 2 - 2;
    SDL_SetRenderDrawColor(renderer, trackColor_.r, trackColor_.g,
                           trackColor_.b, trackColor_.a);
    SDL_FRect track = { (float)(r.x + 4), (float)trackY, (float)(r.w - 8), 4.0f };
    SDL_RenderFillRect(renderer, &track);

    //filled portion
    float range = max_ - min_;
    float ratio = range != 0.f ? (value_ - min_) / range : 0.f;
    int fillW = (int)((r.w - 8) * ratio);
    Color fc = fillColor_;
    if (!enabled_) { fc.r = fc.r * 3 / 4; fc.g = fc.g * 3 / 4; fc.b = fc.b * 3 / 4; }
    SDL_SetRenderDrawColor(renderer, fc.r, fc.g, fc.b, fc.a);
    SDL_FRect fill = { (float)(r.x + 4), (float)trackY, (float)fillW, 4.0f };
    SDL_RenderFillRect(renderer, &fill);

    //thumb
    int thumbX = r.x + 4 + (int)((r.w - 16) * ratio);
    int thumbY = r.y + r.h / 2 - 6;
    Color th = (dragging_ ? thumbHoverColor_ : thumbColor_);
    if (!enabled_) { th.r = th.r * 2 / 3; th.g = th.g * 2 / 3; th.b = th.b * 2 / 3; }
    SDL_SetRenderDrawColor(renderer, th.r, th.g, th.b, th.a);
    SDL_FRect thumb = { (float)thumbX, (float)thumbY, 12.0f, 12.0f };
    SDL_RenderFillRect(renderer, &thumb);
    Color bc = enabled_ ? borderColor_ : Color(borderColor_.r * 2 / 3,
                                                borderColor_.g * 2 / 3,
                                                borderColor_.b * 2 / 3);
    SDL_SetRenderDrawColor(renderer, bc.r, bc.g, bc.b, bc.a);
    SDL_RenderRect(renderer, &thumb);

    for (auto& child : children_)
        child->onRender(renderer);

    dirty_ = false;
}

//==============================================================================
// DropDown
//==============================================================================
DropDown::DropDown(Widget* parent)
    : Widget(parent)
{
    height_ = 24;
    onMouseDown = [this](MouseEvent& e)
    {
        (void)e;
        if (expanded_ && e.y > height_)
        {
            int idx = (e.y - height_) / 20;
            if (idx >= 0 && idx < (int)items_.size())
            {
                selected_ = idx;
                if (onItemSelected) onItemSelected(idx);
            }
        }
        expanded_ = !expanded_;
        if (expanded_)
            listH_ = (int)items_.size() * 20;
        else
            listH_ = 0;
        markDirty();
    };
    onMouseLeave = [this](MouseEvent&)
    {
        //we can't collapse on leave because mouse might be on list item
    };
}

void DropDown::addItem(const std::string& item)
{
    items_.push_back(item);
    markDirty();
}

void DropDown::clear()
{
    items_.clear();
    selected_ = -1;
    expanded_ = false;
    listH_ = 0;
    markDirty();
}

void DropDown::setBackgroundColor(Color color) { bgColor_ = color; markDirty(); }
void DropDown::setTextColor(Color color) { textColor_ = color; markDirty(); }
void DropDown::setBorderColor(Color color) { borderColor_ = color; markDirty(); }
void DropDown::setArrowColor(Color color) { arrowColor_ = color; markDirty(); }
void DropDown::setPopupColor(Color color) { popupColor_ = color; markDirty(); }
void DropDown::setSelectionColor(Color color) { selectionColor_ = color; markDirty(); }

void DropDown::setSelectedIndex(int index)
{
    if (index >= -1 && index < (int)items_.size())
    {
        selected_ = index;
        markDirty();
    }
}

bool DropDown::hitTest(int absX, int absY) const
{
    //include expanded list area
    if (expanded_)
    {
        int ax = absoluteX();
        int ay = absoluteY();
        int h = height_ + listH_;
        if (absX >= ax && absX < ax + width_ && absY >= ay && absY < ay + h)
            return true;
    }
    return Widget::hitTest(absX, absY);
}

void DropDown::onRender(SDL_Renderer* renderer)
{
    if (!visible_) return;

    Rect r = absoluteRect();

    //button area
    {
        detail::ClipGuard btnClip(renderer, r.x, r.y, r.w, r.h);
        SDL_FRect fbc = { (float)r.x, (float)r.y, (float)r.w, (float)r.h };
        SDL_SetRenderDrawColor(renderer, bgColor_.r, bgColor_.g, bgColor_.b, bgColor_.a);
        SDL_RenderFillRect(renderer, &fbc);
        SDL_SetRenderDrawColor(renderer, borderColor_.r, borderColor_.g, borderColor_.b, borderColor_.a);
        SDL_RenderRect(renderer, &fbc);

        //text
        if (selected_ >= 0 && selected_ < (int)items_.size())
            detail::drawText(renderer, items_[selected_].c_str(), r.x + 4, r.y + 4, textColor_);

        //▼ arrow
        detail::drawText(renderer, expanded_ ? "▲" : "▼", r.x + r.w - 14, r.y + 4, arrowColor_);
    }

    //list
    if (expanded_)
    {
        int ly = r.y + r.h;
        int lh = (int)items_.size() * 20;
        detail::ClipGuard listClip(renderer, r.x, ly, r.w, lh);

        SDL_SetRenderDrawColor(renderer, popupColor_.r, popupColor_.g,
                               popupColor_.b, popupColor_.a);
        SDL_FRect listBg = { (float)r.x, (float)ly, (float)r.w, (float)lh };
        SDL_RenderFillRect(renderer, &listBg);

        for (int i = 0; i < (int)items_.size(); i++)
        {
            int iy = ly + i * 20;
            if (i == selected_)
            {
                SDL_SetRenderDrawColor(renderer, selectionColor_.r, selectionColor_.g,
                                       selectionColor_.b, selectionColor_.a);
                SDL_FRect sel = { (float)r.x, (float)iy, (float)r.w, 20.0f };
                SDL_RenderFillRect(renderer, &sel);
            }
            detail::drawText(renderer, items_[i].c_str(), r.x + 4, iy + 2, textColor_);
        }

        SDL_SetRenderDrawColor(renderer, borderColor_.r, borderColor_.g,
                               borderColor_.b, borderColor_.a);
        SDL_RenderRect(renderer, &listBg);
    }

    for (auto& child : children_)
        child->onRender(renderer);

    dirty_ = false;
}

//==============================================================================
// ComboBox
//==============================================================================
ComboBox::ComboBox(Widget* parent)
    : Widget(parent)
{
    height_ = 24;
    focusable_ = true;
    onFocus = [this]() { focused_ = true; markDirty(); };
    onBlur  = [this]() { focused_ = false; if (expanded_) { expanded_ = false; listH_ = 0; } markDirty(); };

    onMouseDown = [this](MouseEvent& e)
    {
        int btnX = width_ - 20;
        if (e.x >= btnX)
        {
            expanded_ = !expanded_;
            if (expanded_) listH_ = (int)items_.size() * 20;
            else           listH_ = 0;
        }
        else if (expanded_ && e.y > height_)
        {
            int idx = (e.y - height_) / 20;
            if (idx >= 0 && idx < (int)items_.size())
            {
                selected_ = idx;
                text_ = items_[idx];
                cursorPos_ = (int)text_.size();
                expanded_ = false;
                listH_ = 0;
                if (onItemSelected) onItemSelected(idx);
            }
        }
        else
        {
            //click inside the text area: place cursor at the clicked column
            int targetX = e.x - 4;
            cursorPos_ = 0;
            while (cursorPos_ < (int)text_.size())
            {
                int next = cursorPos_ + detail::utf8CharLen(text_.c_str() + cursorPos_);
                if (detail::textWidthTo(text_.c_str(), next) > targetX)
                    break;
                cursorPos_ = next;
            }
        }
        markDirty();
    };
}

void ComboBox::addItem(const std::string& item) { items_.push_back(item); markDirty(); }
void ComboBox::clear() { items_.clear(); text_.clear(); selected_ = -1; expanded_ = false; listH_ = 0; markDirty(); }

void ComboBox::setBackgroundColor(Color color) { bgColor_ = color; markDirty(); }
void ComboBox::setTextColor(Color color) { textColor_ = color; markDirty(); }
void ComboBox::setBorderColor(Color color) { borderColor_ = color; markDirty(); }
void ComboBox::setButtonColor(Color color) { buttonColor_ = color; markDirty(); }
void ComboBox::setArrowColor(Color color) { arrowColor_ = color; markDirty(); }
void ComboBox::setPopupColor(Color color) { popupColor_ = color; markDirty(); }
void ComboBox::setSelectionColor(Color color) { selectionColor_ = color; markDirty(); }

void ComboBox::setSelectedIndex(int index)
{
    if (index >= -1 && index < (int)items_.size())
    {
        selected_ = index;
        if (index >= 0) text_ = items_[index];
        cursorPos_ = (int)text_.size();
        markDirty();
    }
}

bool ComboBox::textInputRect(Rect& rect) const
{
    Rect r = absoluteRect();
    int lineH = currentFont()->lineHeight();
    int cx = r.x + 4 + detail::textWidthTo(text_.c_str(), cursorPos_);
    int cy = r.y + (r.h - lineH) / 2;
    rect = { cx, cy, 1, lineH };
    return true;
}

void ComboBox::setText(const std::string& text) { text_ = text; cursorPos_ = (int)text_.size(); markDirty(); }

void ComboBox::insertText(const std::string& text)
{
    if (text.empty()) return;
    text_.insert(cursorPos_, text);
    cursorPos_ += (int)text.size();
    markDirty();
    if (onTextChanged) onTextChanged(text_);
}

void ComboBox::backspace()
{
    if (cursorPos_ <= 0) return;
    int start = detail::utf8Prev(text_, cursorPos_);
    text_.erase(start, cursorPos_ - start);
    cursorPos_ = start;
    markDirty();
    if (onTextChanged) onTextChanged(text_);
}

void ComboBox::deleteForward()
{
    if (cursorPos_ >= (int)text_.size()) return;
    int clen = detail::utf8CharLen(text_.c_str() + cursorPos_);
    text_.erase(cursorPos_, clen);
    markDirty();
    if (onTextChanged) onTextChanged(text_);
}

void ComboBox::moveCursorLeft()
{
    if (cursorPos_ <= 0) return;
    cursorPos_ = detail::utf8Prev(text_, cursorPos_);
    markDirty();
}

void ComboBox::moveCursorRight()
{
    if (cursorPos_ >= (int)text_.size()) return;
    cursorPos_ += detail::utf8CharLen(text_.c_str() + cursorPos_);
    markDirty();
}

void ComboBox::moveCursorHome()
{
    cursorPos_ = 0;
    markDirty();
}

void ComboBox::moveCursorEnd()
{
    cursorPos_ = (int)text_.size();
    markDirty();
}

bool ComboBox::hitTest(int absX, int absY) const
{
    if (expanded_)
    {
        int ax = absoluteX(), ay = absoluteY();
        int h = height_ + listH_;
        if (absX >= ax && absX < ax + width_ && absY >= ay && absY < ay + h) return true;
    }
    return Widget::hitTest(absX, absY);
}

void ComboBox::onRender(SDL_Renderer* renderer)
{
    if (!visible_) return;
    Rect r = absoluteRect();

    //text input area
    {
        detail::ClipGuard inpClip(renderer, r.x, r.y, r.w - 20, r.h);
        SDL_FRect fic = { (float)r.x, (float)r.y, (float)(r.w - 20), (float)r.h };
        SDL_SetRenderDrawColor(renderer, bgColor_.r, bgColor_.g, bgColor_.b, bgColor_.a);
        SDL_RenderFillRect(renderer, &fic);

        int tx = r.x + 4;
        int ty = r.y + (r.h - currentFont()->lineHeight()) / 2;
        detail::drawText(renderer, text_.c_str(), tx, ty, textColor_);

        //cursor
        uint32_t tick = SDL_GetTicks();
        if (focused_ && (tick / 500) % 2 == 0)
        {
            int cx = tx + detail::textWidthTo(text_.c_str(), cursorPos_);
            SDL_SetRenderDrawColor(renderer, textColor_.r, textColor_.g,
                                   textColor_.b, textColor_.a);
            SDL_RenderLine(renderer, cx, r.y + 3, cx, r.y + r.h - 3);
        }
    }

    //dropdown button
    {
        detail::ClipGuard btnClip(renderer, r.x + r.w - 20, r.y, 20, r.h);
        SDL_FRect fbc = { (float)(r.x + r.w - 20), (float)r.y, 20.0f, (float)r.h };
        SDL_SetRenderDrawColor(renderer, buttonColor_.r, buttonColor_.g,
                               buttonColor_.b, buttonColor_.a);
        SDL_RenderFillRect(renderer, &fbc);
        detail::drawText(renderer, "▼", r.x + r.w - 14, r.y + 4, arrowColor_);
    }

    //border
    SDL_FRect border = { (float)r.x, (float)r.y, (float)r.w, (float)r.h };
    SDL_SetRenderDrawColor(renderer, borderColor_.r, borderColor_.g,
                           borderColor_.b, borderColor_.a);
    SDL_RenderRect(renderer, &border);

    //list
    if (expanded_)
    {
        int ly = r.y + r.h;
        int lh = (int)items_.size() * 20;
        detail::ClipGuard listClip(renderer, r.x, ly, r.w, lh);
        SDL_FRect flc = { (float)r.x, (float)ly, (float)r.w, (float)lh };
        SDL_SetRenderDrawColor(renderer, popupColor_.r, popupColor_.g,
                               popupColor_.b, popupColor_.a);
        SDL_RenderFillRect(renderer, &flc);
        for (int i = 0; i < (int)items_.size(); i++)
        {
            int iy = ly + i * 20;
            if (i == selected_)
            {
                SDL_SetRenderDrawColor(renderer, selectionColor_.r, selectionColor_.g,
                                       selectionColor_.b, selectionColor_.a);
                SDL_FRect sel = { (float)r.x, (float)iy, (float)r.w, 20.0f };
                SDL_RenderFillRect(renderer, &sel);
            }
            detail::drawText(renderer, items_[i].c_str(), r.x + 4, iy + 2, textColor_);
        }
        SDL_SetRenderDrawColor(renderer, borderColor_.r, borderColor_.g,
                               borderColor_.b, borderColor_.a);
        SDL_RenderRect(renderer, &flc);
    }

    for (auto& child : children_) child->onRender(renderer);
    dirty_ = false;
}

//==============================================================================
// TextArea
//==============================================================================
TextArea::TextArea(Widget* parent)
    : ScrollView(parent)
{
    focusable_ = true;
    scrollbarV(true);
    scrollbarH(true);

    auto baseDown = onMouseDown; //ScrollView scrollbar handler
    onMouseDown = [this, baseDown](MouseEvent& e)
    {
        baseDown(e);
        if (draggingScrollbar()) return;

        int line = 0, col = 0;
        lineColAt(e.x, e.y, line, col);

        bool shift = (SDL_GetModState() & SDL_KMOD_SHIFT) != 0;
        if (shift)
        {
            if (selAnchorLine_ < 0)
            {
                selAnchorLine_ = cursorLine_;
                selAnchorCol_  = cursorCol_;
            }
        }
        else
        {
            selAnchorLine_ = line;
            selAnchorCol_  = col;
        }

        cursorLine_ = line;
        cursorCol_  = col;
        mouseSelecting_ = true;
        cursorVisible_ = true;
        cursorTick_ = SDL_GetTicks();
        ensureCursorVisible();
        markDirty();
    };

    auto baseMove = onMouseMove;
    onMouseMove = [this, baseMove](MouseEvent& e)
    {
        baseMove(e);
        if (draggingScrollbar() || !mouseSelecting_) return;

        //auto-scroll while dragging outside the viewport
        int lineH = lineHeight();
        if (e.y < 0) setScrollOffset(scrollX_, scrollY_ - lineH);
        else if (e.y >= height_) setScrollOffset(scrollX_, scrollY_ + lineH);
        if (e.x < 0) setScrollOffset(scrollX_ - 16, scrollY_);
        else if (e.x >= width_) setScrollOffset(scrollX_ + 16, scrollY_);

        int line = 0, col = 0;
        lineColAt(e.x, e.y, line, col);

        cursorLine_ = line;
        cursorCol_  = col;
        cursorVisible_ = true;
        cursorTick_ = SDL_GetTicks();
        ensureCursorVisible();
        markDirty();
    };

    auto baseUp = onMouseUp;
    onMouseUp = [this, baseUp](MouseEvent& e)
    {
        baseUp(e);
        mouseSelecting_ = false;
    };

    onFocus = [this]()
    {
        cursorVisible_ = true;
        cursorTick_ = SDL_GetTicks();
        markDirty();
    };
    onBlur = [this]()
    {
        cursorVisible_ = false;
        mouseSelecting_ = false;
        markDirty();
    };
}

void TextArea::setText(const std::string& text)
{
    text_ = text;
    cursorLine_ = 0;
    cursorCol_  = 0;
    selAnchorLine_ = -1;
    selAnchorCol_  = -1;
    mouseSelecting_ = false;
    cursorVisible_ = false;
    clampCursor();
    refreshLayout();
}

void TextArea::setTextColor(Color color) { textColor_ = color; markDirty(); }
void TextArea::setBackgroundColor(Color color) { bgColor_ = color; markDirty(); }
void TextArea::setCursorColor(Color color) { cursorColor_ = color; markDirty(); }
void TextArea::setBorderColor(Color color) { borderColor_ = color; markDirty(); }
void TextArea::setReadOnly(bool ro) { readOnly_ = ro; markDirty(); }
void TextArea::setSelectionColor(Color color) { selColor_ = color; markDirty(); }
void TextArea::setLineSpacing(int px)
{
    lineSpacing_ = px;
    refreshLayout();
}
void TextArea::setLetterSpacing(int px)
{
    letterSpacing_ = px;
    refreshLayout();
}

bool TextArea::hasSelection() const
{
    return selAnchorLine_ >= 0 &&
           (selAnchorLine_ != cursorLine_ || selAnchorCol_ != cursorCol_);
}

void TextArea::selectionBounds(int& sl, int& sc, int& el, int& ec) const
{
    if (selAnchorLine_ < 0)
    {
        sl = el = cursorLine_;
        sc = ec = cursorCol_;
        return;
    }

    int al = selAnchorLine_, ac = selAnchorCol_;
    if (al < cursorLine_ || (al == cursorLine_ && ac <= cursorCol_))
    {
        sl = al; sc = ac;
        el = cursorLine_; ec = cursorCol_;
    }
    else
    {
        sl = cursorLine_; sc = cursorCol_;
        el = al; ec = ac;
    }
}

std::string TextArea::selectedText() const
{
    if (!hasSelection()) return {};

    int sl, sc, el, ec;
    selectionBounds(sl, sc, el, ec);

    std::size_t a = offsetAt(sl, sc);
    std::size_t b = offsetAt(el, ec);
    if (b <= a) return {};
    return text_.substr(a, b - a);
}

void TextArea::selectAll()
{
    auto lines = splitLines();
    selAnchorLine_ = 0;
    selAnchorCol_  = 0;
    cursorLine_ = (int)lines.size() - 1;
    cursorCol_  = (int)lines[cursorLine_].size();
    cursorVisible_ = true;
    cursorTick_ = SDL_GetTicks();
    ensureCursorVisible();
    markDirty();
}

void TextArea::clearSelection()
{
    if (selAnchorLine_ >= 0)
    {
        selAnchorLine_ = -1;
        selAnchorCol_  = -1;
        markDirty();
    }
}

bool TextArea::deleteSelection()
{
    if (!hasSelection() || readOnly_) return false;

    int sl, sc, el, ec;
    selectionBounds(sl, sc, el, ec);

    std::size_t a = offsetAt(sl, sc);
    std::size_t b = offsetAt(el, ec);
    if (b > a)
        text_.erase(a, b - a);

    cursorLine_ = sl;
    cursorCol_  = sc;
    selAnchorLine_ = -1;
    selAnchorCol_  = -1;
    mouseSelecting_ = false;
    cursorTick_ = SDL_GetTicks();
    refreshLayout();
    return true;
}

std::vector<std::string> TextArea::splitLines() const
{
    std::vector<std::string> lines;
    const char* p = text_.c_str();
    const char* start = p;
    while (*p)
    {
        if (*p == '\n')
        {
            lines.emplace_back(start, p - start);
            p++;
            start = p;
        }
        else p++;
    }
    lines.emplace_back(start, p - start);
    if (lines.empty()) lines.emplace_back("");
    return lines;
}

void TextArea::clampCursor()
{
    auto lines = splitLines();
    if (cursorLine_ < 0) cursorLine_ = 0;
    if (cursorLine_ >= (int)lines.size()) cursorLine_ = (int)lines.size() - 1;

    const std::string& ln = lines[cursorLine_];
    if (cursorCol_ < 0) cursorCol_ = 0;
    if (cursorCol_ > (int)ln.size()) cursorCol_ = (int)ln.size();

    while (cursorCol_ > 0 && cursorCol_ < (int)ln.size()
           && ((uint8_t)ln[cursorCol_] & 0xC0) == 0x80)
        --cursorCol_;
}

std::size_t TextArea::offsetAt(int line, int col) const
{
    int l = 0;
    std::size_t lineStart = 0;
    for (std::size_t i = 0; i <= text_.size(); ++i)
    {
        if (l == line)
            return lineStart + (std::size_t)col;
        if (i < text_.size() && text_[i] == '\n')
        {
            ++l;
            lineStart = i + 1;
        }
    }
    return text_.size();
}

void TextArea::lineColAt(int localX, int localY, int& line, int& col) const
{
    auto lines = splitLines();
    if (lines.empty())
    {
        line = 0;
        col = 0;
        return;
    }

    int lineH = lineHeight();
    line = (localY + scrollY_) / lineH;
    if (line < 0) line = 0;
    if (line >= (int)lines.size()) line = (int)lines.size() - 1;

    const std::string& ln = lines[line];
    int targetX = localX + scrollX_ - lineNumberGutterWidth() - 4;
    if (targetX < 0) targetX = 0;
    int x = 0;
    int chars = 0;
    col = 0;
    while (col < (int)ln.size())
    {
        const char* prev = ln.c_str() + col;
        const char* p = prev;
        uint32_t cp = detail::decodeUTF8(p);
        int next = (int)(p - ln.c_str());
        if (chars > 0)
            x += letterSpacing_;
        int cw = glyphAdvance(cp);
        if (x + cw > targetX)
            break;
        x += cw;
        col = next;
        ++chars;
    }
}

int TextArea::lineHeight() const
{
    return std::max(1, currentFont()->lineHeight() + lineSpacing_);
}

int TextArea::lineNumberGutterWidth() const
{
    if (!showLineNumbers_)
        return 0;

    int n = (int)splitLines().size();
    int digits = 1;
    while (n >= 10)
    {
        n /= 10;
        ++digits;
    }

    int dw = currentFont()->advance('0');
    if (dw <= 0) dw = 8;
    return std::max(24, digits * dw + 16);
}

int TextArea::glyphAdvance(uint32_t codepoint) const
{
    return currentFont()->advance(codepoint);
}

int TextArea::prefixWidth(const std::string& line, int bytePos) const
{
    int w = 0;
    int chars = 0;
    int consumed = 0;
    const char* p = line.c_str();

    while (*p && consumed < bytePos)
    {
        const char* prev = p;
        uint32_t cp = detail::decodeUTF8(p);
        int bytes = (int)(p - prev);
        if (consumed + bytes > bytePos) break;

        if (chars > 0)
            w += letterSpacing_;
        w += glyphAdvance(cp);
        consumed += bytes;
        ++chars;
    }

    return std::max(0, w);
}

int TextArea::lineWidth(const std::string& line) const
{
    return prefixWidth(line, (int)line.size());
}

void TextArea::drawLine(SDL_Renderer* renderer, const std::string& line,
                        int x, int y, Color color) const
{
    if (letterSpacing_ == 0)
    {
        detail::drawText(renderer, line.c_str(), x, y, color);
        return;
    }

    const char* p = line.c_str();
    int chars = 0;
    while (*p)
    {
        const char* prev = p;
        uint32_t cp = detail::decodeUTF8(p);
        std::string glyph(prev, p);

        if (chars > 0)
            x += letterSpacing_;
        detail::drawText(renderer, glyph.c_str(), x, y, color);
        x += glyphAdvance(cp);
        ++chars;
    }
}

void TextArea::refreshLayout()
{
    auto lines = splitLines();
    int gutter = lineNumberGutterWidth();
    int maxW = width_;
    for (auto& l : lines)
        maxW = std::max(maxW, gutter + lineWidth(l) + 8);

    int lineH = lineHeight();
    setContentSize(maxW, (int)lines.size() * lineH);
    clampCursor();
    ensureCursorVisible();
    markDirty();
}

void TextArea::ensureCursorVisible()
{
    auto lines = splitLines();
    if (lines.empty()) return;
    if (cursorLine_ < 0) cursorLine_ = 0;
    if (cursorLine_ >= (int)lines.size()) cursorLine_ = (int)lines.size() - 1;

    int lineH  = lineHeight();
    int top    = cursorLine_ * lineH;
    int bottom = top + lineH;
    if (top < scrollY_)
        setScrollOffset(scrollX_, top);
    else if (bottom > scrollY_ + height_)
        setScrollOffset(scrollX_, bottom - height_);

    int cx = lineNumberGutterWidth() + 4 +
             prefixWidth(lines[cursorLine_], cursorCol_);
    if (cx < scrollX_)
        setScrollOffset(cx - 4, scrollY_);
    else if (cx > scrollX_ + width_ - 8)
        setScrollOffset(cx - width_ + 8, scrollY_);
}

void TextArea::setCursor(int line, int col)
{
    cursorLine_ = line;
    cursorCol_  = col;
    selAnchorLine_ = -1;
    selAnchorCol_  = -1;
    clampCursor();
    cursorVisible_ = true;
    cursorTick_ = SDL_GetTicks();
    ensureCursorVisible();
    markDirty();
}

void TextArea::insertText(const std::string& text)
{
    if (readOnly_ || text.empty()) return;

    if (hasSelection())
        deleteSelection();

    clampCursor();
    std::size_t off = offsetAt(cursorLine_, cursorCol_);
    text_.insert(off, text);

    std::size_t lastNl = text.rfind('\n');
    if (lastNl == std::string::npos)
    {
        cursorCol_ += (int)text.size();
    }
    else
    {
        int newlines = 0;
        for (char c : text)
            if (c == '\n') ++newlines;
        cursorLine_ += newlines;
        cursorCol_ = (int)(text.size() - lastNl - 1);
    }

    selAnchorLine_ = -1;
    selAnchorCol_  = -1;
    cursorVisible_ = true;
    cursorTick_ = SDL_GetTicks();
    refreshLayout();
}

void TextArea::insertNewline()
{
    if (readOnly_) return;

    std::string insert = "\n";
    if (autoIndent_)
    {
        auto lines = splitLines();
        const std::string& ln = lines[cursorLine_];
        int limit = std::min(cursorCol_, (int)ln.size());

        int indentEnd = 0;
        while (indentEnd < limit &&
               (ln[indentEnd] == ' ' || ln[indentEnd] == '\t'))
            ++indentEnd;

        std::string indent = ln.substr(0, (std::size_t)indentEnd);

        //A line ending in an opening bracket gets one extra level. This is
        //deliberately simple and does not try to understand the language.
        int significantEnd = limit;
        while (significantEnd > 0 &&
               (ln[significantEnd - 1] == ' ' ||
                ln[significantEnd - 1] == '\t'))
            --significantEnd;
        if (significantEnd > 0)
        {
            char last = ln[significantEnd - 1];
            if (last == '{' || last == '[' || last == '(')
            {
                if (indentWithSpaces_)
                    indent.append((std::size_t)tabWidth_, ' ');
                else
                    indent.push_back('\t');
            }
        }

        insert += indent;
    }

    insertText(insert);
}

void TextArea::backspace()
{
    if (readOnly_) return;
    if (deleteSelection()) return;

    clampCursor();
    if (cursorCol_ > 0)
    {
        auto lines = splitLines();
        const std::string& ln = lines[cursorLine_];
        int start = detail::utf8Prev(ln, cursorCol_);

        //Inside converted indentation, remove a whole soft-tab stop instead
        //of a single space. Real tabs are still removed one at a time.
        if (smartBackspace_ && tabWidth_ > 0)
        {
            bool indentation = true;
            for (int i = 0; i < cursorCol_; ++i)
            {
                if (ln[i] != ' ' && ln[i] != '\t')
                {
                    indentation = false;
                    break;
                }
            }

            if (indentation)
            {
                int visual = 0;
                for (int i = 0; i < cursorCol_; ++i)
                {
                    if (ln[i] == '\t')
                        visual += tabWidth_ - (visual % tabWidth_);
                    else
                        ++visual;
                }

                int target = visual > 0
                    ? ((visual - 1) / tabWidth_) * tabWidth_
                    : 0;
                int newCol = cursorCol_;
                while (newCol > 0 && visual > target &&
                       ln[newCol - 1] == ' ')
                {
                    --newCol;
                    --visual;
                }
                if (newCol < cursorCol_)
                    start = newCol;
            }
        }

        std::size_t off = offsetAt(cursorLine_, start);
        text_.erase(off, (std::size_t)(cursorCol_ - start));
        cursorCol_ = start;
    }
    else if (cursorLine_ > 0)
    {
        auto lines = splitLines();
        int prevLen = (int)lines[cursorLine_ - 1].size();
        std::size_t off = offsetAt(cursorLine_, 0);
        if (off > 0 && text_[off - 1] == '\n')
            text_.erase(off - 1, 1);
        --cursorLine_;
        cursorCol_ = prevLen;
    }

    cursorTick_ = SDL_GetTicks();
    refreshLayout();
}

void TextArea::deleteForward()
{
    if (readOnly_) return;
    if (deleteSelection()) return;

    clampCursor();
    auto lines = splitLines();
    const std::string& ln = lines[cursorLine_];
    if (cursorCol_ < (int)ln.size())
    {
        int clen = detail::utf8CharLen(ln.c_str() + cursorCol_);
        text_.erase(offsetAt(cursorLine_, cursorCol_), (std::size_t)clen);
    }
    else if (cursorLine_ + 1 < (int)lines.size())
    {
        std::size_t off = offsetAt(cursorLine_, cursorCol_);
        if (off < text_.size() && text_[off] == '\n')
            text_.erase(off, 1);
    }

    cursorTick_ = SDL_GetTicks();
    refreshLayout();
}

void TextArea::moveCursorLeft(bool select)
{
    if (select)
    {
        if (selAnchorLine_ < 0) { selAnchorLine_ = cursorLine_; selAnchorCol_ = cursorCol_; }
    }
    else { selAnchorLine_ = -1; selAnchorCol_ = -1; }

    clampCursor();
    auto lines = splitLines();
    if (cursorCol_ > 0)
    {
        cursorCol_ = detail::utf8Prev(lines[cursorLine_], cursorCol_);
    }
    else if (cursorLine_ > 0)
    {
        --cursorLine_;
        cursorCol_ = (int)lines[cursorLine_].size();
    }

    cursorTick_ = SDL_GetTicks();
    ensureCursorVisible();
    markDirty();
}

void TextArea::moveCursorRight(bool select)
{
    if (select)
    {
        if (selAnchorLine_ < 0) { selAnchorLine_ = cursorLine_; selAnchorCol_ = cursorCol_; }
    }
    else { selAnchorLine_ = -1; selAnchorCol_ = -1; }

    clampCursor();
    auto lines = splitLines();
    if (cursorCol_ < (int)lines[cursorLine_].size())
    {
        cursorCol_ += detail::utf8CharLen(
            lines[cursorLine_].c_str() + cursorCol_);
    }
    else if (cursorLine_ + 1 < (int)lines.size())
    {
        ++cursorLine_;
        cursorCol_ = 0;
    }

    cursorTick_ = SDL_GetTicks();
    ensureCursorVisible();
    markDirty();
}

void TextArea::lineColForOffset(std::size_t off, int& line, int& col) const
{
    if (off > text_.size())
        off = text_.size();

    line = 0;
    std::size_t start = 0;
    for (std::size_t i = 0; i < off; ++i)
    {
        if (text_[i] == '\n')
        {
            ++line;
            start = i + 1;
        }
    }
    col = (int)(off - start);
}

bool TextArea::isWordCodepoint(uint32_t cp)
{
    if (cp < 0x80)
        return std::isalnum((int)cp) != 0 || cp == '_';

    //Treat CJK punctuation and ideographic space as separators.
    if (cp == 0x3000) return false;
    if (cp >= 0x3001 && cp <= 0x303F) return false;
    if (cp >= 0xFF01 && cp <= 0xFF0F) return false;
    if (cp >= 0xFF1A && cp <= 0xFF20) return false;
    if (cp >= 0xFF3B && cp <= 0xFF40) return false;
    if (cp >= 0xFF5B && cp <= 0xFF65) return false;

    return std::iswspace((wint_t)cp) == 0 &&
           std::iswpunct((wint_t)cp) == 0;
}

void TextArea::moveCursorWordLeft(bool select)
{
    if (select)
    {
        if (selAnchorLine_ < 0) { selAnchorLine_ = cursorLine_; selAnchorCol_ = cursorCol_; }
    }
    else { selAnchorLine_ = -1; selAnchorCol_ = -1; }

    clampCursor();
    std::size_t p = offsetAt(cursorLine_, cursorCol_);

    //Skip separators to the left, then the preceding word.
    while (p > 0)
    {
        int prev = detail::utf8Prev(text_, (int)p);
        const char* q = text_.c_str() + prev;
        uint32_t cp = detail::decodeUTF8(q);
        if (isWordCodepoint(cp)) break;
        p = (std::size_t)prev;
    }
    while (p > 0)
    {
        int prev = detail::utf8Prev(text_, (int)p);
        const char* q = text_.c_str() + prev;
        uint32_t cp = detail::decodeUTF8(q);
        if (!isWordCodepoint(cp)) break;
        p = (std::size_t)prev;
    }

    lineColForOffset(p, cursorLine_, cursorCol_);
    cursorTick_ = SDL_GetTicks();
    ensureCursorVisible();
    markDirty();
}

void TextArea::moveCursorWordRight(bool select)
{
    if (select)
    {
        if (selAnchorLine_ < 0) { selAnchorLine_ = cursorLine_; selAnchorCol_ = cursorCol_; }
    }
    else { selAnchorLine_ = -1; selAnchorCol_ = -1; }

    clampCursor();
    std::size_t p = offsetAt(cursorLine_, cursorCol_);

    //Skip separators to the right, then the following word.
    while (p < text_.size())
    {
        const char* start = text_.c_str() + p;
        const char* q = start;
        uint32_t cp = detail::decodeUTF8(q);
        if (isWordCodepoint(cp)) break;
        p += (std::size_t)(q - start);
    }
    while (p < text_.size())
    {
        const char* start = text_.c_str() + p;
        const char* q = start;
        uint32_t cp = detail::decodeUTF8(q);
        if (!isWordCodepoint(cp)) break;
        p += (std::size_t)(q - start);
    }

    lineColForOffset(p, cursorLine_, cursorCol_);
    cursorTick_ = SDL_GetTicks();
    ensureCursorVisible();
    markDirty();
}

void TextArea::moveCursorUp(bool select)
{
    if (select)
    {
        if (selAnchorLine_ < 0) { selAnchorLine_ = cursorLine_; selAnchorCol_ = cursorCol_; }
    }
    else { selAnchorLine_ = -1; selAnchorCol_ = -1; }

    clampCursor();
    if (cursorLine_ <= 0) return;

    auto lines = splitLines();
    --cursorLine_;

    const std::string& ln = lines[cursorLine_];
    if (cursorCol_ > (int)ln.size()) cursorCol_ = (int)ln.size();
    while (cursorCol_ > 0 && cursorCol_ < (int)ln.size()
           && ((uint8_t)ln[cursorCol_] & 0xC0) == 0x80)
        --cursorCol_;

    cursorTick_ = SDL_GetTicks();
    ensureCursorVisible();
    markDirty();
}

void TextArea::moveCursorDown(bool select)
{
    if (select)
    {
        if (selAnchorLine_ < 0) { selAnchorLine_ = cursorLine_; selAnchorCol_ = cursorCol_; }
    }
    else { selAnchorLine_ = -1; selAnchorCol_ = -1; }

    clampCursor();
    auto lines = splitLines();
    if (cursorLine_ + 1 >= (int)lines.size()) return;

    ++cursorLine_;

    const std::string& ln = lines[cursorLine_];
    if (cursorCol_ > (int)ln.size()) cursorCol_ = (int)ln.size();
    while (cursorCol_ > 0 && cursorCol_ < (int)ln.size()
           && ((uint8_t)ln[cursorCol_] & 0xC0) == 0x80)
        --cursorCol_;

    cursorTick_ = SDL_GetTicks();
    ensureCursorVisible();
    markDirty();
}

void TextArea::moveCursorHome(bool select)
{
    if (select)
    {
        if (selAnchorLine_ < 0) { selAnchorLine_ = cursorLine_; selAnchorCol_ = cursorCol_; }
    }
    else { selAnchorLine_ = -1; selAnchorCol_ = -1; }

    clampCursor();
    cursorCol_ = 0;
    cursorTick_ = SDL_GetTicks();
    ensureCursorVisible();
    markDirty();
}

void TextArea::moveCursorEnd(bool select)
{
    if (select)
    {
        if (selAnchorLine_ < 0) { selAnchorLine_ = cursorLine_; selAnchorCol_ = cursorCol_; }
    }
    else { selAnchorLine_ = -1; selAnchorCol_ = -1; }

    clampCursor();
    cursorCol_ = (int)splitLines()[cursorLine_].size();
    cursorTick_ = SDL_GetTicks();
    ensureCursorVisible();
    markDirty();
}

bool TextArea::textInputRect(Rect& rect) const
{
    auto lines = splitLines();
    if (lines.empty()) return false;

    int line = cursorLine_;
    if (line < 0) line = 0;
    if (line >= (int)lines.size()) line = (int)lines.size() - 1;

    int col = cursorCol_;
    if (col < 0) col = 0;
    if (col > (int)lines[line].size()) col = (int)lines[line].size();

    int lineH = lineHeight();
    Rect r = absoluteRect();
    int cx = r.x + lineNumberGutterWidth() + 4 - scrollX_ +
             prefixWidth(lines[line], col);
    int cy = r.y + line * lineH - scrollY_;
    rect = { cx, cy, 1, lineH };
    return true;
}

void TextArea::onFontChanged()
{
    refreshLayout();
    markDirty();
}

void TextArea::onRender(SDL_Renderer* renderer)
{
    if (!visible_) return;

    Rect r = absoluteRect();
    SDL_FRect fc = { (float)r.x, (float)r.y, (float)r.w, (float)r.h };

    detail::ClipGuard clip(renderer, r.x, r.y, r.w, r.h);

    //background
    SDL_SetRenderDrawColor(renderer, bgColor_.r, bgColor_.g, bgColor_.b, bgColor_.a);
    SDL_RenderFillRect(renderer, &fc);

    //border
    SDL_SetRenderDrawColor(renderer, borderColor_.r, borderColor_.g,
                           borderColor_.b, borderColor_.a);
    SDL_RenderRect(renderer, &fc);

    auto lines = splitLines();
    int maxW = 0;
    int lineH = lineHeight();
    int gutter = lineNumberGutterWidth();

    bool sel = hasSelection();
    int selSL = 0, selSC = 0, selEL = 0, selEC = 0;
    if (sel)
        selectionBounds(selSL, selSC, selEL, selEC);

    for (int i = 0; i < (int)lines.size(); i++)
    {
        int ly = r.y + i * lineH - scrollY_;
        if (ly + lineH < r.y || ly > r.y + r.h) continue;  //skip offscreen

        if (sel && i >= selSL && i <= selEL)
        {
            const std::string& ln = lines[i];
            int c0 = (i == selSL) ? selSC : 0;
            int c1 = (i == selEL) ? selEC : (int)ln.size();
            if (c1 < c0) c1 = c0;

            int x0 = r.x + gutter + 4 - scrollX_ + prefixWidth(ln, c0);
            int x1 = r.x + gutter + 4 - scrollX_ + prefixWidth(ln, c1);
            if (x1 == x0) x1 = x0 + 4; //empty line / newline marker

            SDL_FRect selr = { (float)x0, (float)ly, (float)(x1 - x0), (float)lineH };
            SDL_SetRenderDrawColor(renderer,
                selColor_.r, selColor_.g, selColor_.b, selColor_.a);
            SDL_RenderFillRect(renderer, &selr);
        }

        drawLine(renderer, lines[i], r.x + gutter + 4 - scrollX_, ly, textColor_);

        int lw = gutter + lineWidth(lines[i]);
        if (lw > maxW) maxW = lw;
    }

    //cursor
    if (cursorVisible_ && cursorLine_ >= 0 && cursorLine_ < (int)lines.size())
    {
        uint32_t elapsed = SDL_GetTicks() - cursorTick_;
        if ((elapsed / 500) % 2 == 0)
        {
            int col = cursorCol_;
            if (col > (int)lines[cursorLine_].size())
                col = (int)lines[cursorLine_].size();

            int cx = r.x + gutter + 4 - scrollX_ +
                     prefixWidth(lines[cursorLine_], col);
            int cy = r.y + cursorLine_ * lineH - scrollY_;
            if (cy + lineH >= r.y && cy <= r.y + r.h)
            {
                SDL_SetRenderDrawColor(renderer, cursorColor_.r, cursorColor_.g,
                                       cursorColor_.b, cursorColor_.a);
                SDL_RenderLine(renderer, cx, cy, cx, cy + lineH);
            }
        }
    }

    //line-number gutter: draw after the text so horizontally scrolled text
    //is clipped underneath it.
    if (gutter > 0)
    {
        SDL_FRect gb = { (float)(r.x + 1), (float)(r.y + 1),
                         (float)(gutter - 1), (float)(r.h - 2) };
        SDL_SetRenderDrawColor(renderer,
            (Uint8)(bgColor_.r * 4 / 5),
            (Uint8)(bgColor_.g * 4 / 5),
            (Uint8)(bgColor_.b * 4 / 5),
            255);
        SDL_RenderFillRect(renderer, &gb);

        SDL_SetRenderDrawColor(renderer, borderColor_.r, borderColor_.g,
                               borderColor_.b, borderColor_.a);
        SDL_RenderLine(renderer, (float)(r.x + gutter - 1), (float)r.y,
                       (float)(r.x + gutter - 1), (float)(r.y + r.h));

        int first = std::max(0, scrollY_ / lineH);
        int last = std::min((int)lines.size(),
                            (scrollY_ + r.h) / lineH + 1);
        Color numberColor = getWindow()
            ? getWindow()->theme().mutedText
            : Color(115, 120, 130);
        for (int i = first; i < last; ++i)
        {
            std::string num = std::to_string(i + 1);
            int nw = detail::textWidth(num.c_str());
            int nx = r.x + gutter - 8 - nw;
            int ny = r.y + i * lineH - scrollY_;
            detail::drawText(renderer, num.c_str(), nx, ny, numberColor);
        }
    }

    //update content size (grow and shrink with the viewport/font)
    int totalH = (int)lines.size() * lineH;
    int newW = std::max(width_, maxW + 8);
    int newH = std::max(height_, totalH);
    if (newW != contentW_ || newH != contentH_)
        setContentSize(newW, newH);

    for (auto& child : children_)
    {
        child->setRenderOffset(-scrollX_, -scrollY_);
        child->onRender(renderer);
        child->setRenderOffset(0, 0);
    }

    renderScrollbars(renderer);

    dirty_ = false;
}

//==============================================================================
// ScrollView
//==============================================================================
ScrollView::ScrollView(Widget* parent)
    : Widget(parent)
{
    onMouseDown = [this](MouseEvent& e)
    {
        //vertical scrollbar
        if (showVScroll_ && contentH_ > height_ && e.x >= width_ - 10)
        {
            float ratio = (float)height_ / contentH_;
            int thumbH = (int)(height_ * ratio);
            if (thumbH < 10) thumbH = 10;
            sbDragging_ = true;
            sbVertical_ = true;
            sbDragMouse_ = e.y;
            sbDragScroll_ = scrollY_;
            return;
        }
        //horizontal scrollbar
        if (showHScroll_ && contentW_ > width_ && e.y >= height_ - 10)
        {
            int sbW = width_;
            if (showVScroll_ && contentH_ > height_) sbW -= 10;
            if (e.x < sbW)
            {
                float ratio = (float)width_ / contentW_;
                int thumbW = (int)(ratio * (showVScroll_ && contentH_ > height_ ? width_ - 10 : width_));
                if (thumbW < 10) thumbW = 10;
                sbDragging_ = true;
                sbVertical_ = false;
                sbDragMouse_ = e.x;
                sbDragScroll_ = scrollX_;
                return;
            }
        }
    };
    onMouseMove = [this](MouseEvent& e)
    {
        if (!sbDragging_) return;
        int delta = sbVertical_ ? (e.y - sbDragMouse_) : (e.x - sbDragMouse_);
        int range = sbVertical_ ? contentH_ : contentW_;
        int view  = sbVertical_ ? height_ : width_;
        int newOff = sbDragScroll_ + delta * range / view;
        if (sbVertical_)
            setScrollOffset(scrollX_, newOff);
        else
            setScrollOffset(newOff, scrollY_);
    };
    onMouseUp = [this](MouseEvent&)
    {
        sbDragging_ = false;
    };
}

void ScrollView::setContentSize(int w, int h)
{
    contentW_ = w;
    contentH_ = h;
    markDirty();
}

void ScrollView::setScrollbarColors(Color track, Color thumb)
{
    scrollTrackColor_ = track;
    scrollThumbColor_ = thumb;
    markDirty();
}

void ScrollView::setScrollOffset(int x, int y)
{
    int maxX = std::max(0, contentW_ - width_);
    int maxY = std::max(0, contentH_ - height_);
    scrollX_ = std::max(0, std::min(x, maxX));
    scrollY_ = std::max(0, std::min(y, maxY));
    markDirty();
}

void ScrollView::onRender(SDL_Renderer* renderer)
{
    if (!visible_) return;

    Rect r = absoluteRect();
    detail::ClipGuard clip(renderer, r.x, r.y, r.w, r.h);

    //Render children with a temporary offset; geometry and dirty flags stay intact.
    for (auto& child : children_)
    {
        child->setRenderOffset(-scrollX_, -scrollY_);
        child->onRender(renderer);
        child->setRenderOffset(0, 0);
    }

    renderScrollbars(renderer);
    dirty_ = false;
}

void ScrollView::renderScrollbars(SDL_Renderer* renderer)
{
    Rect r = absoluteRect();

    //vertical scrollbar
    if (showVScroll_ && contentH_ > r.h)
    {
        int sbW  = 10;
        int sbX  = r.x + r.w - sbW;
        int sbY  = r.y;
        int sbH  = r.h;

        //track
        SDL_SetRenderDrawColor(renderer, scrollTrackColor_.r, scrollTrackColor_.g,
                               scrollTrackColor_.b, scrollTrackColor_.a);
        SDL_FRect track = { (float)sbX, (float)sbY, (float)sbW, (float)sbH };
        SDL_RenderFillRect(renderer, &track);

        //thumb
        float ratio = (float)r.h / contentH_;
        int thumbH  = (int)(sbH * ratio);
        if (thumbH < 10) thumbH = 10;
        int thumbY  = sbY + (int)(scrollY_ * ratio);
        SDL_SetRenderDrawColor(renderer, scrollThumbColor_.r, scrollThumbColor_.g,
                               scrollThumbColor_.b, scrollThumbColor_.a);
        SDL_FRect thumb = { (float)(sbX + 1), (float)thumbY, (float)(sbW - 2), (float)thumbH };
        SDL_RenderFillRect(renderer, &thumb);
    }

    //horizontal scrollbar
    if (showHScroll_ && contentW_ > r.w)
    {
        int sbH  = 10;
        int sbX  = r.x;
        int sbY  = r.y + r.h - sbH;
        int sbW  = r.w;

        if (showVScroll_ && contentH_ > r.h)
            sbW -= 10;  //leave room for vsb

        SDL_SetRenderDrawColor(renderer, scrollTrackColor_.r, scrollTrackColor_.g,
                               scrollTrackColor_.b, scrollTrackColor_.a);
        SDL_FRect track = { (float)sbX, (float)sbY, (float)sbW, (float)sbH };
        SDL_RenderFillRect(renderer, &track);

        float ratio = (float)r.w / contentW_;
        int thumbW  = (int)(sbW * ratio);
        if (thumbW < 10) thumbW = 10;
        int thumbX  = sbX + (int)(scrollX_ * ratio);
        SDL_SetRenderDrawColor(renderer, scrollThumbColor_.r, scrollThumbColor_.g,
                               scrollThumbColor_.b, scrollThumbColor_.a);
        SDL_FRect thumb = { (float)thumbX, (float)(sbY + 1), (float)thumbW, (float)(sbH - 2) };
        SDL_RenderFillRect(renderer, &thumb);
    }
}

void ScrollView::onMouseWheel(int delta)
{
    setScrollOffset(scrollX_, scrollY_ - delta * 20);
}

bool ScrollView::hitTestScrollbar(int absX, int absY) const
{
    int ax = absoluteX();
    int ay = absoluteY();

    if (showVScroll_ && contentH_ > height_
        && absX >= ax + width_ - 10 && absX < ax + width_)
        return absY >= ay && absY < ay + height_;

    if (showHScroll_ && contentW_ > width_)
    {
        int sbW = width_;
        if (showVScroll_ && contentH_ > height_) sbW -= 10;
        if (absX >= ax && absX < ax + sbW)
            return absY >= ay + height_ - 10 && absY < ay + height_;
    }

    return false;
}

bool ScrollView::hitTest(int absX, int absY) const
{
    if (!Widget::hitTest(absX, absY))
        return false;

    //children are handled by Application::hitTest with scroll offset compensation
    return true;
}

//==============================================================================
// Window
//==============================================================================
Window::Window(const char* title, int w, int h, TitleBarMode mode)
{
    title_          = title ? title : "";
    clientWidth_    = w;
    clientHeight_   = h;
    titleBarMode_   = mode;

    //Keep the widget tree alive even if SDL window creation fails.
    root_ = std::unique_ptr<Widget>(new Widget(nullptr));
    root_->window_ = this;
    root_->setSize(w, h);

    int sdlFlags = 0;
    if (mode == TitleBarMode::System || resizable_)
        sdlFlags |= SDL_WINDOW_RESIZABLE;

    sdlWindow_ = SDL_CreateWindow(title_.c_str(), w, h, sdlFlags);
    if (!sdlWindow_)
    {
        SDL_Log("sdlgui: SDL_CreateWindow failed: %s", SDL_GetError());
        return;
    }

    renderer_ = SDL_CreateRenderer(sdlWindow_, nullptr);
    if (!renderer_)
    {
        SDL_Log("sdlgui: SDL_CreateRenderer failed: %s", SDL_GetError());
        SDL_DestroyWindow(sdlWindow_);
        sdlWindow_ = nullptr;
        return;
    }

    if (!SDL_SetRenderVSync(renderer_, 1))
        SDL_Log("sdlgui: SDL_SetRenderVSync failed: %s", SDL_GetError());
    SDL_StartTextInput(sdlWindow_);

    if (mode != TitleBarMode::System)
    {
        SDL_SetWindowBordered(sdlWindow_, false);
        if (mode == TitleBarMode::Custom)
            titleBarHeight_ = 30;
        root_->setPosition(0, titleBarHeight_);
        root_->setSize(w, h - titleBarHeight_);

        //Borderless windows need explicit edge hit-testing for OS resize.
        SDL_SetWindowHitTest(sdlWindow_, &Window::hitTest, this);
    }
}

SDL_HitTestResult SDLCALL Window::hitTest(SDL_Window* sdlWindow,
                                          const SDL_Point* area, void* data)
{
    auto* self = static_cast<Window*>(data);
    if (!self || !self->resizable_)
        return SDL_HITTEST_NORMAL;

    int w = 0, h = 0;
    SDL_GetWindowSize(sdlWindow, &w, &h);

    const int b = 6;
    bool left   = area->x < b;
    bool right  = area->x >= w - b;
    bool top    = area->y < b;
    bool bottom = area->y >= h - b;

    if (top && left)     return SDL_HITTEST_RESIZE_TOPLEFT;
    if (top && right)    return SDL_HITTEST_RESIZE_TOPRIGHT;
    if (bottom && left)  return SDL_HITTEST_RESIZE_BOTTOMLEFT;
    if (bottom && right) return SDL_HITTEST_RESIZE_BOTTOMRIGHT;
    if (left)            return SDL_HITTEST_RESIZE_LEFT;
    if (right)           return SDL_HITTEST_RESIZE_RIGHT;
    if (top)             return SDL_HITTEST_RESIZE_TOP;
    if (bottom)          return SDL_HITTEST_RESIZE_BOTTOM;

    return SDL_HITTEST_NORMAL;
}

Window::~Window()
{
    //Widgets (and their SDL textures) must be destroyed before the renderer.
    menuBar_ = nullptr;
    root_.reset();
    contextMenu_.reset();
    messageBox_.reset();

    if (renderer_)
    {
        detail::clearTextCache(renderer_);
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }
    if (sdlWindow_) { SDL_DestroyWindow(sdlWindow_); sdlWindow_ = nullptr; }
}

void Window::forgetWidget(Widget* w)
{
    if (menuBar_ == w)
        menuBar_ = nullptr;
}

bool Window::anyContinuousRedraw(Widget* w) const
{
    if (!w || !w->visible())
        return false;

    if (w->wantsContinuousRedraw())
        return true;

    for (auto& child : w->children())
    {
        if (anyContinuousRedraw(child.get()))
            return true;
    }

    return false;
}

bool Window::needsRedraw() const
{
    if (!sdlWindow_ || !renderer_ || !root_)
        return false;

#ifdef SDLGUI_ALWAYS_REDRAW
    return true;
#else
    return redraw_ || root_->dirty() || anyContinuousRedraw(root_.get());
#endif
}

void Window::setTitle(const char* title)
{
    title_ = title ? title : "";
    if (sdlWindow_)
        SDL_SetWindowTitle(sdlWindow_, title_.c_str());
    redraw_ = true;
}

void Window::setResizable(bool resiz)
{
    resizable_ = resiz;
    if (sdlWindow_)
        SDL_SetWindowResizable(sdlWindow_, resiz ? true : false);
    redraw_ = true;
}

void Window::setTitleBarMode(TitleBarMode mode)
{
    titleBarMode_ = mode;
    redraw_ = true;
    if (sdlWindow_)
    {
        switch (mode)
        {
        case TitleBarMode::System:
            SDL_SetWindowBordered(sdlWindow_, true);
            SDL_SetWindowResizable(sdlWindow_, resizable_ ? true : false);
            titleBarHeight_ = 0;
            break;
        case TitleBarMode::Custom:
            SDL_SetWindowBordered(sdlWindow_, false);
            titleBarHeight_ = 30;
            break;
        case TitleBarMode::Hidden:
            SDL_SetWindowBordered(sdlWindow_, false);
            titleBarHeight_ = 0;
            break;
        }
    }
    else
    {
        titleBarHeight_ = (mode == TitleBarMode::Custom) ? 30 : 0;
    }
    root_->setPosition(0, titleBarHeight_);
    root_->setSize(clientWidth_, clientHeight_ - titleBarHeight_);

    if (!shown_ && sdlWindow_)
    {
        SDL_ShowWindow(sdlWindow_);
        shown_ = true;
    }
}

void Window::setTitleBarHeight(int h)
{
    titleBarHeight_ = h;
    root_->setPosition(0, titleBarHeight_);
    root_->setSize(clientWidth_, clientHeight_ - titleBarHeight_);
    redraw_ = true;
}

void Window::onResized(int w, int h)
{
    clientWidth_  = w;
    clientHeight_ = h;
    root_->setSize(w, h - titleBarHeight_);
    root_->setPosition(0, titleBarHeight_);
    redraw_ = true;
}

bool Window::handleTitleBarEvent(const SDL_Event& ev, int mx, int my)
{
    if (titleBarMode_ != TitleBarMode::Custom || !sdlWindow_)
        return false;

    //MessageBox 激活时禁止标题栏交互
    if (messageBox_ && messageBox_->isActive())
        return true;

    int bh  = titleBarHeight_;
    int bsz = bh;
    int btnCnt = resizable_ ? 3 : 2;
    int minX   = clientWidth_ - bsz * btnCnt;
    int maxX   = resizable_ ? (clientWidth_ - bsz * 2) : -1;
    int clsX   = clientWidth_ - bsz;

    //hover tracking
    if (ev.type == SDL_EVENT_MOUSE_MOTION)
    {
        int oldHover = hoveredBtn_;
        hoveredBtn_ = 0;
        if (my >= 0 && my < bh && mx >= minX)
        {
            if      (mx >= clsX)                  hoveredBtn_ = 3;
            else if (resizable_ && mx >= maxX)    hoveredBtn_ = 2;
            else                                   hoveredBtn_ = 1;
        }
        if (oldHover != hoveredBtn_)
            redraw_ = true;
    }

    //Title-bar drags and button releases must still be delivered when the
    //pointer leaves the title bar area (mouse capture keeps the events coming).
    if (my >= bh && !dragging_
        && !(ev.type == SDL_EVENT_MOUSE_BUTTON_UP && ev.button.button == SDL_BUTTON_LEFT))
        return false;

    //dragging + double-click maximize
    if (ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN && ev.button.button == SDL_BUTTON_LEFT)
    {
        SDL_CaptureMouse(true);
        redraw_ = true;

        if (mx >= minX)
        {
            if      (mx >= clsX)               pressedTitleBtn_ = 3;
            else if (resizable_ && mx >= maxX) pressedTitleBtn_ = 2;
            else                               pressedTitleBtn_ = 1;
            return true;  //buttons consume but no drag
        }

        if (ev.button.clicks == 2 && resizable_)
        {
            if (maximized_)
                SDL_RestoreWindow(sdlWindow_);
            else
                SDL_MaximizeWindow(sdlWindow_);
            maximized_ = !maximized_;
            return true;
        }

        dragging_ = true;
        SDL_GetWindowPosition(sdlWindow_, &dragStartWinX_, &dragStartWinY_);
        SDL_GetGlobalMouseState(&dragStartMouseX_, &dragStartMouseY_);
        return true;
    }

    if (ev.type == SDL_EVENT_MOUSE_MOTION && dragging_)
    {
        //Use the global mouse position; window-relative coordinates change as
        //the window moves and cause feedback jitter.
        float gx = 0.f, gy = 0.f;
        SDL_GetGlobalMouseState(&gx, &gy);
        int dx = (int)std::lround(gx - dragStartMouseX_);
        int dy = (int)std::lround(gy - dragStartMouseY_);
        SDL_SetWindowPosition(sdlWindow_,
            dragStartWinX_ + dx, dragStartWinY_ + dy);
        return true;
    }

    if (ev.type == SDL_EVENT_MOUSE_BUTTON_UP && ev.button.button == SDL_BUTTON_LEFT)
    {
        bool hadInteraction = dragging_ || pressedTitleBtn_ != 0;
        dragging_ = false;
        SDL_CaptureMouse(false);
        redraw_ = true;

        if (pressedTitleBtn_ != 0)
        {
            int btn = pressedTitleBtn_;
            pressedTitleBtn_ = 0;

            //Only trigger if the pointer is released over the same button.
            int hit = 0;
            if (my >= 0 && my < bh && mx >= minX)
            {
                if      (mx >= clsX)               hit = 3;
                else if (resizable_ && mx >= maxX) hit = 2;
                else                               hit = 1;
            }

            if (hit == btn)
            {
                if (btn == 3)
                {
                    SDL_Event q;
                    q.type = SDL_EVENT_QUIT;
                    SDL_PushEvent(&q);
                }
                else if (btn == 2)
                {
                    if (maximized_)
                        SDL_RestoreWindow(sdlWindow_);
                    else
                        SDL_MaximizeWindow(sdlWindow_);
                    maximized_ = !maximized_;
                }
                else
                {
                    SDL_MinimizeWindow(sdlWindow_);
                }
            }
        }

        return hadInteraction;
    }

    return false;
}

void Window::setBackgroundColor(Color color)
{
    bgColor_ = color;
    redraw_ = true;
}

void Window::setTitleBarColor(Color color)
{
    titleBarColor_ = color;
    redraw_ = true;
}

void Window::setTitleTextColor(Color color)
{
    titleTextColor_ = color;
    redraw_ = true;
}

void Window::setTitleButtonColors(Color normal, Color hover)
{
    titleButtonColor_ = normal;
    titleButtonHoverColor_ = hover;
    redraw_ = true;
}

void Window::setTitleCloseColor(Color color)
{
    titleCloseColor_ = color;
    redraw_ = true;
}

void Window::setTheme(const Theme& theme)
{
    theme_ = theme;
    setBackgroundColor(theme.windowBg);
    setTitleBarColor(theme.titleBarBg);
    setTitleTextColor(theme.titleText);
    setTitleButtonColors(theme.titleButton, theme.titleButtonHover);
    setTitleCloseColor(theme.titleCloseHover);
    applyTheme(*root_, theme);
    if (contextMenu_)
        applyTheme(*contextMenu_, theme);
    if (messageBox_)
        messageBox_->applyTheme(theme);
    requestRedraw();
}

void Window::render()
{
    if (!sdlWindow_ || !renderer_ || !root_)
        return;

    //Blink the caret only while this window owns keyboard focus.
    bool need = needsRedraw();
    if (focused_ && SDL_GetKeyboardFocus() == sdlWindow_)
    {
        uint64_t phase = SDL_GetTicks() / 500;
        if (phase != lastBlinkPhase_)
        {
            lastBlinkPhase_ = phase;
            need = true;
        }
    }

    if (!need)
        return;

    redraw_ = false;

    if (!shown_)
    {
        SDL_ShowWindow(sdlWindow_);
        shown_ = true;
    }

    SDL_SetRenderDrawColor(renderer_, bgColor_.r, bgColor_.g, bgColor_.b, bgColor_.a);
    SDL_RenderClear(renderer_);

    if (titleBarMode_ == TitleBarMode::Custom)
    {
        int bh  = titleBarHeight_;
        int bsz = bh;
        int btnCnt = resizable_ ? 3 : 2;

        int minX = clientWidth_ - bsz * btnCnt;
        int maxX = resizable_ ? (clientWidth_ - bsz * 2) : -1;
        int clsX = clientWidth_ - bsz;

        SDL_FRect tb = { 0, 0, (float)clientWidth_, (float)bh };
        SDL_SetRenderDrawColor(renderer_, titleBarColor_.r, titleBarColor_.g,
                               titleBarColor_.b, titleBarColor_.a);
        SDL_RenderFillRect(renderer_, &tb);
        detail::drawText(renderer_, title_.c_str(), 6,
                         (bh - currentFont()->lineHeight()) / 2, titleTextColor_);

        //--- minimize [_] ---
        SDL_FRect minBtn = { (float)minX, 0, (float)bsz, (float)bsz };
        if (hoveredBtn_ == 1)
        {
            SDL_SetRenderDrawColor(renderer_, titleButtonHoverColor_.r,
                                   titleButtonHoverColor_.g, titleButtonHoverColor_.b,
                                   titleButtonHoverColor_.a);
            SDL_RenderFillRect(renderer_, &minBtn);
            SDL_SetRenderDrawColor(renderer_, titleTextColor_.r, titleTextColor_.g,
                                   titleTextColor_.b, titleTextColor_.a);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer_, titleButtonColor_.r, titleButtonColor_.g,
                                   titleButtonColor_.b, titleButtonColor_.a);
            SDL_RenderFillRect(renderer_, &minBtn);
            SDL_SetRenderDrawColor(renderer_, titleTextColor_.r, titleTextColor_.g,
                                   titleTextColor_.b, titleTextColor_.a);
        }
        SDL_RenderLine(renderer_, minX + 8, bh - 8, minX + bsz - 8, bh - 8);

        //--- maximize [□] (only if resizable) ---
        if (resizable_)
        {
            SDL_FRect maxBtn = { (float)maxX, 0, (float)bsz, (float)bsz };
            if (hoveredBtn_ == 2)
            {
                SDL_SetRenderDrawColor(renderer_, titleButtonHoverColor_.r,
                                       titleButtonHoverColor_.g, titleButtonHoverColor_.b,
                                       titleButtonHoverColor_.a);
                SDL_RenderFillRect(renderer_, &maxBtn);
                SDL_SetRenderDrawColor(renderer_, titleTextColor_.r, titleTextColor_.g,
                                       titleTextColor_.b, titleTextColor_.a);
            }
            else
            {
                SDL_SetRenderDrawColor(renderer_, titleButtonColor_.r, titleButtonColor_.g,
                                       titleButtonColor_.b, titleButtonColor_.a);
                SDL_RenderFillRect(renderer_, &maxBtn);
                SDL_SetRenderDrawColor(renderer_, titleTextColor_.r, titleTextColor_.g,
                                       titleTextColor_.b, titleTextColor_.a);
            }
            if (maximized_)
            {
                SDL_FRect r1 = { (float)(maxX + 4), 4, (float)(bsz - 10), (float)(bsz - 10) };
                SDL_RenderRect(renderer_, &r1);
                SDL_FRect r2 = { (float)(maxX + 7), 7, (float)(bsz - 10), (float)(bsz - 10) };
                SDL_RenderRect(renderer_, &r2);
            }
            else
            {
                SDL_FRect r = { (float)(maxX + 5), 5, (float)(bsz - 10), (float)(bsz - 10) };
                SDL_RenderRect(renderer_, &r);
            }
        }

        //--- close [X] ---
        SDL_FRect clsBtn = { (float)clsX, 0, (float)bsz, (float)bsz };
        if (hoveredBtn_ == 3)
        {
            SDL_SetRenderDrawColor(renderer_, titleCloseColor_.r, titleCloseColor_.g,
                                   titleCloseColor_.b, titleCloseColor_.a);
            SDL_RenderFillRect(renderer_, &clsBtn);
            SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 255);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer_, titleButtonColor_.r, titleButtonColor_.g,
                                   titleButtonColor_.b, titleButtonColor_.a);
            SDL_RenderFillRect(renderer_, &clsBtn);
            SDL_SetRenderDrawColor(renderer_, titleTextColor_.r, titleTextColor_.g,
                                   titleTextColor_.b, titleTextColor_.a);
        }
        SDL_RenderLine(renderer_, clsX + 8, 8, clsX + bsz - 8, bsz - 8);
        SDL_RenderLine(renderer_, clsX + bsz - 8, 8, clsX + 8, bsz - 8);

        //separator
        SDL_SetRenderDrawColor(renderer_, 80, 80, 80, 255);
        SDL_RenderLine(renderer_, 0, bh - 1, clientWidth_, bh - 1);
    }

    root_->onRender(renderer_);

    //keyboard-only focus ring (mouse focus stays clean)
    if (focusRing_ && focused_ && focused_->visible()
        && !dynamic_cast<TextBox*>(focused_))
    {
        Rect fr = focused_->absoluteRect();
        if (fr.w > 4 && fr.h > 4)
        {
            SDL_FRect ring = { (float)(fr.x + 1), (float)(fr.y + 1),
                               (float)(fr.w - 2), (float)(fr.h - 2) };
            SDL_SetRenderDrawColor(renderer_, 80, 140, 220, 255);
            SDL_RenderRect(renderer_, &ring);
        }
    }

    //menu bar dropdown overlay (on top of all regular widgets)
    if (menuBar_)
        menuBar_->renderDropdownOverlay(renderer_);

    //context menu overlay
    if (contextMenu_ && contextMenu_->isActive())
        contextMenu_->renderContextMenu(renderer_);

    //message box overlay (on top of everything)
    if (messageBox_ && messageBox_->isActive())
        messageBox_->renderOverlay(renderer_, clientWidth_, clientHeight_);

    SDL_RenderPresent(renderer_);
}

void Window::showMessageBox(const char* title, const char* message,
                             MessageBoxType type, MessageBoxButtons buttons,
                             std::function<void(MessageBoxResult)> onResult)
{
    if (!messageBox_)
        messageBox_ = std::unique_ptr<MessageBox>(new MessageBox);
    messageBox_->applyTheme(theme_);
    messageBox_->show(title, message, type, buttons, std::move(onResult),
                      clientWidth_, clientHeight_);
    redraw_ = true;
}

MessageBoxResult Window::showMessageBoxBlocking(
    const char* title, const char* message,
    MessageBoxType type, MessageBoxButtons buttons)
{
    MessageBoxResult result = MessageBoxResult::None;
    bool done = false;

    if (!valid())
        return result;

    showMessageBox(title, message, type, buttons,
        [&](MessageBoxResult r) { result = r; done = true; });

    auto* app = Application::instance();
    while (!done && app)
    {
        Uint32 fs = SDL_GetTicks();
        app->processEvents();
        render();
        if (app->fps() > 0)
        {
            Uint32 elapsed = SDL_GetTicks() - fs;
            int wait = (1000 / app->fps()) - (int)elapsed;
            if (wait > 0) SDL_Delay((Uint32)wait);
        }
    }

    if (messageBox_)
        messageBox_->dismiss();
    return result;
}

//==============================================================================
// Application
//==============================================================================
Application::Application()
{
    instance_ = this;
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("sdlgui: SDL_Init failed: %s", SDL_GetError());
        return;
    }
    initialized_ = true;
    SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
}

Application::~Application()
{
    //Destroy windows (and their renderers/widgets) while SDL is still alive.
    windows_.clear();
    if (initialized_)
        SDL_Quit();
    if (instance_ == this)
        instance_ = nullptr;
}

void Application::setFPS(int fps)
{
    targetFps_ = fps;
}

Window& Application::addWindow(const char* title, int w, int h)
{
    auto win = std::unique_ptr<Window>(new Window(title, w, h));
    Window& ref = *win;
    windows_.push_back(std::move(win));
    return ref;
}

void Application::quit()
{
    running_ = false;
}

void Application::refreshFont()
{
    for (auto& win : windows_)
    {
        if (!win->root_) continue;
        win->root_->notifyFontChanged();
        win->root_->relayout();
        win->requestRedraw();
        if (win->focused_)
            updateTextInputArea(win->focused_);
    }
}

void Application::updateTextInputArea(Widget* widget)
{
    if (!widget) return;

    if (!dynamic_cast<TextBox*>(widget) &&
        !dynamic_cast<TextArea*>(widget) &&
        !dynamic_cast<ComboBox*>(widget))
        return;

    Window* win = widget->getWindow();
    if (!win || !win->handle()) return;

    Rect caret;
    if (!widget->textInputRect(caret)) return;

    //Keep the IME anchor inside the focused widget.
    Rect wr = widget->absoluteRect();
    if (wr.w > 0 && caret.x < wr.x) caret.x = wr.x;
    if (wr.w > 0 && caret.x > wr.x + wr.w - 1) caret.x = wr.x + wr.w - 1;
    if (wr.h > 0 && caret.y < wr.y) caret.y = wr.y;
    if (wr.h > 0 && caret.y > wr.y + wr.h - 1) caret.y = wr.y + wr.h - 1;

    if (caret.w < 1) caret.w = 1;
    if (caret.h < 1) caret.h = 1;

    int ww = 0, wh = 0;
    SDL_GetWindowSize(win->handle(), &ww, &wh);
    if (ww > 0)
    {
        if (caret.x < 0) caret.x = 0;
        if (caret.x >= ww) caret.x = ww - 1;
    }
    if (wh > 0)
    {
        if (caret.y < 0) caret.y = 0;
        if (caret.y >= wh) caret.y = wh - 1;
    }

    SDL_Rect area = { caret.x, caret.y, caret.w, caret.h };
    SDL_SetTextInputArea(win->handle(), &area, 0);
}

Theme Theme::light()
{
    Theme t;
    t.windowBg = Color(245, 245, 245);
    t.text = Color(30, 30, 30);
    t.disabledText = Color(150, 150, 150);
    t.mutedText = Color(110, 110, 110);
    t.border = Color(190, 190, 190);

    t.buttonBg = Color(235, 235, 235);
    t.buttonHover = Color(248, 248, 248);
    t.buttonPressed = Color(215, 215, 215);
    t.buttonText = Color(30, 30, 30);
    t.buttonBorder = Color(180, 180, 180);

    t.inputBg = Color(255, 255, 255);
    t.inputText = Color(25, 25, 25);
    t.inputPlaceholder = Color(150, 150, 150);
    t.inputBorder = Color(180, 180, 180);
    t.inputCursor = Color(20, 20, 20);
    t.inputSelection = Color(180, 210, 245);

    t.panelBg = Color(250, 250, 250);
    t.panelBorder = Color(200, 200, 200);
    t.rowHover = Color(232, 238, 246);
    t.rowSelected = Color(190, 215, 245);
    t.rowSelectedText = Color(20, 20, 20);

    t.scrollTrack = Color(235, 235, 235);
    t.scrollThumb = Color(190, 190, 190);

    t.menuBarBg = Color(238, 238, 238);
    t.menuHover = Color(220, 226, 235);
    t.menuText = Color(30, 30, 30);
    t.menuDisabled = Color(150, 150, 150);
    t.menuShortcut = Color(120, 120, 120);
    t.menuPopupBg = Color(255, 255, 255);
    t.menuItemHover = Color(205, 225, 248);
    t.menuAccent = Color(70, 130, 210);

    t.textAreaBg = Color(255, 255, 255);
    t.textAreaText = Color(30, 30, 30);
    t.textAreaCursor = Color(30, 30, 30);
    t.textAreaSelection = Color(190, 215, 245);

    t.sliderTrack = Color(210, 210, 210);
    t.sliderFill = Color(70, 130, 210);
    t.sliderThumb = Color(250, 250, 250);
    t.sliderThumbHover = Color(255, 255, 255);
    t.sliderBorder = Color(150, 150, 150);

    t.titleBarBg = Color(235, 235, 235);
    t.titleText = Color(30, 30, 30);
    t.titleButton = Color(235, 235, 235);
    t.titleButtonHover = Color(215, 215, 215);
    t.titleCloseHover = Color(220, 60, 60);

    t.overlay = Color(0, 0, 0, 90);
    t.dialogBg = Color(255, 255, 255);
    t.dialogBorder = Color(180, 180, 180);
    t.dialogTitleBg = Color(240, 240, 240);
    t.dialogTitleText = Color(30, 30, 30);
    t.dialogText = Color(30, 30, 30);
    t.dialogButton = Color(238, 238, 238);
    t.dialogButtonHover = Color(218, 225, 235);
    t.dialogButtonBorder = Color(180, 180, 180);
    t.dialogButtonText = Color(30, 30, 30);

    t.checkboxBox = Color(255, 255, 255);
    t.checkboxBorder = Color(170, 170, 170);
    t.checkboxCheck = Color(0, 140, 70);
    t.divider = Color(210, 210, 210);
    return t;
}

void applyTheme(Widget& root, const Theme& theme)
{
    if (auto* l = dynamic_cast<Label*>(&root))
    {
        l->setTextColor(theme.text);
    }
    else if (auto* b = dynamic_cast<Button*>(&root))
    {
        b->setTextColor(theme.buttonText);
        b->setBgColor(theme.buttonBg, theme.buttonHover, theme.buttonPressed);
        b->setBorderColor(theme.buttonBorder);
    }
    else if (auto* c = dynamic_cast<CheckBox*>(&root))
    {
        c->setTextColor(theme.text);
        c->setCheckColor(theme.checkboxCheck);
        c->setBoxColor(theme.checkboxBox);
        c->setBorderColor(theme.checkboxBorder);
    }
    else if (auto* tb = dynamic_cast<TextBox*>(&root))
    {
        tb->setBackgroundColor(theme.inputBg);
        tb->setTextColor(theme.inputText);
        tb->setPlaceholderColor(theme.inputPlaceholder);
        tb->setCursorColor(theme.inputCursor);
        tb->setSelectionColor(theme.inputSelection);
        tb->setBorderColor(theme.inputBorder);
    }
    else if (auto* lb = dynamic_cast<ListBox*>(&root))
    {
        lb->setBackgroundColor(theme.panelBg);
        lb->setTextColor(theme.text);
        lb->setSelectionColor(theme.rowSelected);
        lb->setHoverColor(theme.rowHover);
        lb->setBorderColor(theme.panelBorder);
    }
    else if (auto* mb = dynamic_cast<MenuBar*>(&root))
    {
        mb->setBackgroundColor(theme.menuBarBg);
        mb->setTextColor(theme.menuText);
        mb->setHoverColor(theme.menuHover);
        mb->setPopupColor(theme.menuPopupBg);
        mb->setBorderColor(theme.panelBorder);
        mb->setItemHoverColor(theme.menuItemHover);
        mb->setDisabledTextColor(theme.menuDisabled);
        mb->setShortcutColor(theme.menuShortcut);
        mb->setAccentColor(theme.menuAccent);
    }
    else if (auto* cm = dynamic_cast<ContextMenu*>(&root))
    {
        cm->setBackgroundColor(theme.menuPopupBg);
        cm->setTextColor(theme.menuText);
        cm->setHoverColor(theme.menuItemHover);
        cm->setBorderColor(theme.panelBorder);
        cm->setDisabledTextColor(theme.menuDisabled);
        cm->setSeparatorColor(theme.panelBorder);
    }
    else if (auto* sl = dynamic_cast<Slider*>(&root))
    {
        sl->setTrackColor(theme.sliderTrack);
        sl->setFillColor(theme.sliderFill);
        sl->setThumbColor(theme.sliderThumb, theme.sliderThumbHover);
        sl->setBorderColor(theme.sliderBorder);
    }
    else if (auto* dd = dynamic_cast<DropDown*>(&root))
    {
        dd->setBackgroundColor(theme.panelBg);
        dd->setTextColor(theme.text);
        dd->setBorderColor(theme.panelBorder);
        dd->setArrowColor(theme.mutedText);
        dd->setPopupColor(theme.menuPopupBg);
        dd->setSelectionColor(theme.rowSelected);
    }
    else if (auto* cb = dynamic_cast<ComboBox*>(&root))
    {
        cb->setBackgroundColor(theme.inputBg);
        cb->setTextColor(theme.inputText);
        cb->setBorderColor(theme.inputBorder);
        cb->setButtonColor(theme.buttonBg);
        cb->setArrowColor(theme.mutedText);
        cb->setPopupColor(theme.menuPopupBg);
        cb->setSelectionColor(theme.rowSelected);
    }
    else if (auto* ta = dynamic_cast<TextArea*>(&root))
    {
        ta->setBackgroundColor(theme.textAreaBg);
        ta->setTextColor(theme.textAreaText);
        ta->setCursorColor(theme.textAreaCursor);
        ta->setSelectionColor(theme.textAreaSelection);
        ta->setBorderColor(theme.panelBorder);
    }

    if (auto* sv = dynamic_cast<ScrollView*>(&root))
        sv->setScrollbarColors(theme.scrollTrack, theme.scrollThumb);

    if (auto* sp = dynamic_cast<SplitView*>(&root))
        sp->setDividerColor(theme.divider);

    for (auto& child : root.children())
        applyTheme(*child, theme);
}

void applyTheme(Window& window, const Theme& theme)
{
    window.setTheme(theme);
}

void Application::setTheme(const Theme& theme)
{
    for (auto& win : windows_)
        if (win) win->setTheme(theme);
}

void applyTheme(Application& app, const Theme& theme)
{
    app.setTheme(theme);
}

void Application::run()
{
    if (!initialized_)
        return;

    bool hasWindow = false;
    for (auto& win : windows_)
    {
        if (win->valid()) { hasWindow = true; break; }
    }
    if (!hasWindow)
    {
        running_ = false;
        return;
    }

    running_ = true;

    while (running_)
    {
        Uint64 frameStart = SDL_GetTicks();

        processEvents();

        for (auto& win : windows_)
            win->render();

        if (targetFps_ > 0)
        {
            Uint64 elapsed = SDL_GetTicks() - frameStart;
            int wait = (1000 / targetFps_) - (int)elapsed;
            if (wait > 0)
            {
                //Sleep until input arrives (or the frame deadline), instead of
                //always waiting a full frame. This keeps dragging responsive.
                SDL_Event ev;
                if (SDL_WaitEventTimeout(&ev, wait))
                    processEvent(ev);
            }
        }
    }
}

void Application::processEvents()
{
    SDL_Event ev;
    while (SDL_PollEvent(&ev))
        processEvent(ev);
}

void Application::processEvent(const SDL_Event& ev)
{
    switch (ev.type)
    {
        case SDL_EVENT_QUIT:
            running_ = false;
            break;
        case SDL_EVENT_MOUSE_MOTION:
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
            handleMouseEvent(ev);
            updateTextInputArea(focused_);
            break;
        case SDL_EVENT_TEXT_EDITING:
            //Composition is drawn by the OS/IME; committed text arrives as TEXT_INPUT.
            updateTextInputArea(focused_);
            break;
        case SDL_EVENT_TEXT_INPUT:
            {
                Widget* target = focused_;
                if (Window* win = findWindowByID(ev.text.windowID))
                    target = win->focused_;

                if (target)
                {
                    if (auto* tb = dynamic_cast<TextBox*>(target))
                    {
                        tb->insertText(ev.text.text);
                    }
                    else if (auto* ta = dynamic_cast<TextArea*>(target))
                    {
                        ta->insertText(ev.text.text);
                    }
                    else if (auto* cb = dynamic_cast<ComboBox*>(target))
                    {
                        cb->insertText(ev.text.text);
                    }

                    updateTextInputArea(target);
                }
            }
            break;
        case SDL_EVENT_MOUSE_WHEEL:
            {
                auto* win = findWindowByID(ev.wheel.windowID);
                if (!win) break;
                Widget* target = hitTest(&win->root(), (int)ev.wheel.mouse_x, (int)ev.wheel.mouse_y);
                bool shift = (SDL_GetModState() & SDL_KMOD_SHIFT) != 0;
                float wx = ev.wheel.x;
                float wy = ev.wheel.y;

                while (target)
                {
                    //Horizontal wheel / trackpad scrolls a single-line TextBox.
                    //Shift+vertical wheel also scrolls it horizontally.
                    if (auto* tb = dynamic_cast<TextBox*>(target))
                    {
                        int dx = 0;
                        if (wx != 0.0f)
                            dx = -(int)(wx * 20.0f);
                        else if (shift && wy != 0.0f)
                            dx = (int)(wy * 20.0f);

                        if (dx != 0 && tb->scrollByPixels(dx))
                            break;
                    }

                    if (dynamic_cast<ScrollView*>(target) ||
                        dynamic_cast<ListBox*>(target))
                    {
                        target->onMouseWheel((int)wy);
                        break;
                    }
                    target = target->parent();
                }

                updateTextInputArea(focused_);
            }
            break;
        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP:
            handleKeyEvent(ev);
            updateTextInputArea(focused_);
            break;
        case SDL_EVENT_WINDOW_RESIZED:
            {
                auto* win = findWindowByID(ev.window.windowID);
                if (win)
                {
                    win->onResized(ev.window.data1, ev.window.data2);
                    if (win->focused_)
                        updateTextInputArea(win->focused_);
                }
            }
            break;
        case SDL_EVENT_WINDOW_EXPOSED:
        case SDL_EVENT_WINDOW_SHOWN:
        case SDL_EVENT_WINDOW_RESTORED:
            {
                auto* win = findWindowByID(ev.window.windowID);
                if (win)
                    win->requestRedraw();
            }
            break;
        case SDL_EVENT_WINDOW_FOCUS_GAINED:
            {
                auto* win = findWindowByID(ev.window.windowID);
                if (win)
                    win->requestRedraw();
                if (win && win->focused_)
                    setFocus(win->focused_, win->focusRing_);
                else
                    focused_ = nullptr;
            }
            break;
        case SDL_EVENT_WINDOW_FOCUS_LOST:
            pressedWidget_ = nullptr;
            SDL_CaptureMouse(false);
            updateHovered(nullptr);
            if (auto* win = findWindowByID(ev.window.windowID))
                win->requestRedraw();
            break;
        default:
            if (onUserEvent) onUserEvent(ev);
            break;
    }
}

Window* Application::findWindowByID(Uint32 id)
{
    for (auto& win : windows_)
    {
        if (!win->handle())
            continue;
        if (SDL_GetWindowID(win->handle()) == id)
            return win.get();
    }
    return nullptr;
}

Widget* Application::hitTest(Widget* w, int absX, int absY)
{
    if (!w || !w->visible())
        return nullptr;

    if (!w->hitTest(absX, absY))
        return nullptr;

    //Scrollbars sit on top of the ScrollView's children.
    auto* sv = dynamic_cast<ScrollView*>(w);
    if (sv && sv->hitTestScrollbar(absX, absY))
        return w;

    //check children in reverse order (topmost first)
    for (auto it = w->children().rbegin(); it != w->children().rend(); ++it)
    {
        auto* child = it->get();

        //If parent is a ScrollView, compensate click coords for scroll offset
        //(children are visually at parent_abs - scrollX/Y, so click needs +scrollX/Y)
        int cx = absX, cy = absY;
        if (sv)
        {
            cx = absX + sv->scrollX();
            cy = absY + sv->scrollY();
        }

        Widget* result = hitTest(child, cx, cy);
        if (result) return result;
    }

    return w;
}

void Application::updateHovered(Widget* target)
{
    if (target != hovered_)
    {
        if (hovered_ && hovered_->onMouseLeave)
        {
            MouseEvent ev;
            hovered_->onMouseLeave(ev);
        }
        hovered_ = target;
        if (hovered_ && hovered_->onMouseEnter)
        {
            MouseEvent ev;
            hovered_->onMouseEnter(ev);
        }
    }
}

void Application::widgetDestroyed(Widget* w)
{
    if (auto* win = w->getWindow())
    {
        if (win->focused_ == w)
        {
            win->focused_ = nullptr;
            win->focusRing_ = false;
        }
    }

    if (focused_ == w)       focused_ = nullptr;
    if (hovered_ == w)       hovered_ = nullptr;
    if (pressedWidget_ == w) pressedWidget_ = nullptr;
}

void Application::closePopups(Widget* w, int absX, int absY)
{
    if (!w) return;

    if (w->isPopupOpen() && !w->hitTest(absX, absY))
        w->closePopup();

    for (auto& child : w->children())
        closePopups(child.get(), absX, absY);
}

void Application::setFocus(Widget* w, bool fromKeyboard)
{
    if (focused_ == w)
    {
        if (w)
        {
            if (auto* win = w->getWindow())
            {
                if (win->focusRing_ != fromKeyboard)
                {
                    win->focusRing_ = fromKeyboard;
                    win->requestRedraw();
                }
            }
            updateTextInputArea(w);
        }
        return;
    }

    auto isTextEditable = [](Widget* widget)
    {
        return dynamic_cast<TextBox*>(widget) != nullptr
            || dynamic_cast<ComboBox*>(widget) != nullptr
            || dynamic_cast<TextArea*>(widget) != nullptr;
    };

    //deactivate old focus
    if (focused_)
    {
        focused_->markDirty();

        if (auto* oldWin = focused_->getWindow())
        {
            if (oldWin->focused_ == focused_)
                oldWin->focused_ = nullptr;
            oldWin->focusRing_ = false;
            oldWin->requestRedraw();
        }

        if (isTextEditable(focused_))
        {
            if (auto* win = focused_->getWindow())
                if (win->handle())
                    SDL_StopTextInput(win->handle());
        }

        auto* oldTb = dynamic_cast<TextBox*>(focused_);
        if (oldTb) { oldTb->hasFocus_ = false; oldTb->markDirty(); }
        if (focused_->onBlur) focused_->onBlur();
    }

    focused_ = w;

    //activate new focus
    if (focused_)
    {
        if (auto* newWin = focused_->getWindow())
        {
            newWin->focused_ = focused_;
            newWin->focusRing_ = fromKeyboard;
            newWin->requestRedraw();
        }

        focused_->markDirty();
        auto* newTb = dynamic_cast<TextBox*>(focused_);
        if (newTb)
        {
            newTb->hasFocus_ = true;
            newTb->focusTick_ = SDL_GetTicks();
            newTb->markDirty();
        }
        if (focused_->onFocus) focused_->onFocus();

        //Keep the IME candidate window next to the focused editor.
        if (isTextEditable(focused_))
        {
            if (auto* win = focused_->getWindow())
            {
                if (win->handle())
                    SDL_StartTextInput(win->handle());
            }
            updateTextInputArea(focused_);
        }
    }
}

void Application::handleMouseEvent(const SDL_Event& ev)
{
    Uint32 winID;
    int    mx, my;
    uint8_t button = 0;
    uint8_t clicks = 0;

    if (ev.type == SDL_EVENT_MOUSE_MOTION)
    {
        winID  = ev.motion.windowID;
        mx     = ev.motion.x;
        my     = ev.motion.y;
    }
    else
    {
        winID  = ev.button.windowID;
        mx     = ev.button.x;
        my     = ev.button.y;
        button = ev.button.button;
        clicks = ev.button.clicks;
    }

    Window* win = findWindowByID(winID);
    if (!win && pressedWidget_)
        win = pressedWidget_->getWindow();
    if (!win || !win->valid()) return;

    //Overlays are drawn on top; give them a frame whenever they receive input.
    if ((win->messageBox_ && win->messageBox_->isActive()) ||
        (win->contextMenu_ && win->contextMenu_->isActive()) ||
        (win->menuBar_ && win->menuBar_->isMenuOpen()))
        win->requestRedraw();

    //ContextMenu handling: dismiss on outside click, execute on item click
    if (win->contextMenu_ && win->contextMenu_->isActive() && (ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN || ev.type == SDL_EVENT_MOUSE_BUTTON_UP))
    {
        if (ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
        {
            //check if click is inside the menu
            if (win->contextMenu_->hitTest(mx, my))
            {
                //item click
                int idx = (my - win->contextMenu_->absoluteY()) / 22;
                if (idx >= 0 && idx < (int)win->contextMenu_->items_.size())
                {
                    auto& item = win->contextMenu_->items_[idx];
                    if (!item.separator && item.enabled && item.onClick)
                        item.onClick();
                }
                win->contextMenu_->dismiss();
                return;
            }
            //click outside → dismiss
            win->contextMenu_->dismiss();
        }
    }

    //MessageBox handling: intercept all events when active
    if (win->messageBox_ && win->messageBox_->isActive() &&
        (ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN || ev.type == SDL_EVENT_MOUSE_BUTTON_UP ||
         ev.type == SDL_EVENT_MOUSE_MOTION))
    {
        if (ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
        {
            int btn = win->messageBox_->hitTestButton(mx, my);
            if (btn == -1)
            {
                //click on overlay → dismiss
                auto cb = std::move(win->messageBox_->onResult_);
                win->messageBox_->dismiss();
                if (cb) cb(MessageBoxResult::None);
            }
            else if (btn >= 0)
            {
                //click on button → execute
                win->messageBox_->pressedBtnIdx_ = btn;
            }
        }
        else if (ev.type == SDL_EVENT_MOUSE_BUTTON_UP)
        {
            if (win->messageBox_->pressedBtnIdx_ >= 0)
            {
                int btn = win->messageBox_->pressedBtnIdx_;
                int cur = win->messageBox_->hitTestButton(mx, my);
                win->messageBox_->pressedBtnIdx_ = -1;
                if (btn == cur)
                {
                    auto& bi = win->messageBox_->buttons_[btn];
                    auto cb = std::move(win->messageBox_->onResult_);
                    win->messageBox_->dismiss();
                    if (cb) cb(bi.result);
                }
            }
        }
        else if (ev.type == SDL_EVENT_MOUSE_MOTION)
        {
            int oldHover = win->messageBox_->hoveredBtn_;
            int btn = win->messageBox_->hitTestButton(mx, my);
            win->messageBox_->hoveredBtn_ = btn;
            if (oldHover != btn)
                win->requestRedraw();
        }
        return;
    }

    //MenuBar menu-open handling: route clicks inside dropdown to MenuBar
    //(the dropdown extends below the bar, covering other widgets)
    if (win->menuBar_ && win->menuBar_->isMenuOpen() &&
        (ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN || ev.type == SDL_EVENT_MOUSE_BUTTON_UP || ev.type == SDL_EVENT_MOUSE_MOTION))
    {
        if (win->menuBar_->hitTest(mx, my))
        {
            MouseEvent me;
            me.x       = mx - win->menuBar_->absoluteX();
            me.y       = my - win->menuBar_->absoluteY();
            me.screenX = mx;
            me.screenY = my;
            me.button  = button;
            me.clicks  = clicks;

            if (ev.type == SDL_EVENT_MOUSE_MOTION && win->menuBar_->onMouseMove)
                win->menuBar_->onMouseMove(me);
            if (ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN && win->menuBar_->onMouseDown)
                win->menuBar_->onMouseDown(me);
            if (ev.type == SDL_EVENT_MOUSE_BUTTON_UP && win->menuBar_->onMouseUp)
                win->menuBar_->onMouseUp(me);
            return;
        }
        else
        {
            win->menuBar_->closeAll();
        }
    }

    //let window handle title bar events first
    if (win->handleTitleBarEvent(ev, mx, my))
        return;

    if (ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
        closePopups(&win->root(), mx, my);

    //MOUSE_BUTTON_UP: always dispatch to pressedWidget_ regardless of hitTest
    if (ev.type == SDL_EVENT_MOUSE_BUTTON_UP)
    {
        SDL_CaptureMouse(false);

        Widget* pressed = pressedWidget_;
        if (pressed)
        {
            MouseEvent me;
            me.x       = mx - pressed->absoluteX();
            me.y       = my - pressed->absoluteY();
            me.screenX = mx;
            me.screenY = my;
            me.button  = button;
            me.clicks  = clicks;
            if (pressed->onMouseUp) pressed->onMouseUp(me);

            //The callback may have destroyed the widget.
            if (pressedWidget_ == pressed)
            {
                if (button == SDL_BUTTON_LEFT && pressed->hitTest(mx, my))
                    if (pressed->onClick) pressed->onClick(me);
                pressedWidget_ = nullptr;
            }
        }
        return;
    }

    //While a button is held, motion belongs to the pressed widget (drag capture).
    if (ev.type == SDL_EVENT_MOUSE_MOTION && pressedWidget_)
    {
        Widget* pressed = pressedWidget_;
        MouseEvent me;
        me.x       = mx - pressed->absoluteX();
        me.y       = my - pressed->absoluteY();
        me.screenX = mx;
        me.screenY = my;
        me.button  = button;
        me.clicks  = clicks;
        if (pressed->onMouseMove) pressed->onMouseMove(me);
        return;
    }

    //root widget already offsets by titleBarHeight_ internally
    Widget* target = hitTest(&win->root(), mx, my);
    if (!target)
    {
        updateHovered(nullptr);
        return;
    }

    updateHovered(target);

    //onMouseEnter may have destroyed the widget.
    if (hovered_ != target)
        return;

    MouseEvent me;
    me.x       = mx - target->absoluteX();
    me.y       = my - target->absoluteY();
    me.screenX = mx;
    me.screenY = my;
    me.button  = button;
    me.clicks  = clicks;

    switch (ev.type)
    {
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        pressedWidget_ = target;
        SDL_CaptureMouse(true);

        if (target->focusable())
            setFocus(target, false);
        else
            setFocus(nullptr, false);

        //setFocus may have destroyed the widget via onBlur.
        if (pressedWidget_ != target)
            break;

        if (target->onMouseDown) target->onMouseDown(me);
        //right-click → context menu
        if (pressedWidget_ == target && me.button == SDL_BUTTON_RIGHT)
        {
            if (!win->contextMenu_)
            {
                win->contextMenu_ = std::unique_ptr<ContextMenu>(new ContextMenu(nullptr));
                applyTheme(*win->contextMenu_, win->theme_);
            }
            win->contextMenu_->clear();
            if (target->onContextMenu)
                target->onContextMenu(me, *win->contextMenu_);
            if (!win->contextMenu_->items_.empty())
            {
                win->contextMenu_->show(mx, my);
                win->requestRedraw();
            }
        }
        break;

    case SDL_EVENT_MOUSE_MOTION:
        if (target->onMouseMove) target->onMouseMove(me);
        break;
    }
}

static void collectFocusable(Widget* w, std::vector<Widget*>& out)
{
    if (!w || !w->visible() || !w->enabled()) return;
    if (w->focusable()) out.push_back(w);
    for (auto& child : w->children())
        collectFocusable(child.get(), out);
}

void Application::handleKeyEvent(const SDL_Event& ev)
{
    Window* win = findWindowByID(ev.key.windowID);
    Widget* focus = focused_;
    if (win) focus = win->focused_;
    if (!win && focus) win = focus->getWindow();

    //Modal overlays consume keyboard input.
    if (win && (win->messageBox_ && win->messageBox_->isActive()))
    {
        if (ev.type == SDL_EVENT_KEY_DOWN)
        {
            SDL_Keycode k = ev.key.key;

            if (k == SDLK_ESCAPE)
            {
                auto cb = std::move(win->messageBox_->onResult_);
                win->messageBox_->dismiss();
                if (cb) cb(MessageBoxResult::None);
                win->requestRedraw();
            }
            else if (k == SDLK_RETURN || k == SDLK_KP_ENTER)
            {
                if (!win->messageBox_->buttons_.empty())
                {
                    MessageBoxResult result = win->messageBox_->buttons_[0].result;
                    auto cb = std::move(win->messageBox_->onResult_);
                    win->messageBox_->dismiss();
                    if (cb) cb(result);
                    win->requestRedraw();
                }
            }
        }
        return;
    }

    if (win && win->contextMenu_ && win->contextMenu_->isActive())
    {
        if (ev.type == SDL_EVENT_KEY_DOWN && ev.key.key == SDLK_ESCAPE)
        {
            win->contextMenu_->dismiss();
            win->requestRedraw();
        }
        return;
    }

    if (win && win->menuBar_ && win->menuBar_->isMenuOpen())
    {
        if (ev.type == SDL_EVENT_KEY_DOWN && ev.key.key == SDLK_ESCAPE)
            win->menuBar_->closeAll();
        return;
    }

    //Tab / Shift+Tab focus traversal (opt-in).
    if (tabTraversal_ && ev.type == SDL_EVENT_KEY_DOWN &&
        ev.key.key == SDLK_TAB && win)
    {
        std::vector<Widget*> list;
        collectFocusable(&win->root(), list);
        if (!list.empty())
        {
            bool back = (ev.key.mod & SDL_KMOD_SHIFT) != 0;
            std::size_t idx = 0;

            auto it = std::find(list.begin(), list.end(), focus);
            if (it == list.end())
                idx = back ? list.size() - 1 : 0;
            else if (back)
                idx = (it == list.begin()) ? list.size() - 1
                                           : (std::size_t)(it - list.begin()) - 1;
            else
                idx = ((std::size_t)(it - list.begin()) + 1) % list.size();

            setFocus(list[idx], true);
            return;
        }
    }

    if (!focus) return;

    KeyEvent ke;
    ke.key  = ev.key.key;
    ke.mod  = ev.key.mod;
    ke.text = 0;

    if (ev.type == SDL_EVENT_KEY_DOWN)
    {
        //handle text input for TextBox
        auto* tb = dynamic_cast<TextBox*>(focus);
        if (tb)
        {
            TextEditAction action = tb->keyMap().find(ev.key.key, ev.key.mod);

            if (action == TextEditAction::SelectAll)
            {
                tb->selectAll();
            }
            else if (action == TextEditAction::Copy)
            {
                if (tb->hasSelection())
                    SDL_SetClipboardText(tb->selectedText().c_str());
            }
            else if (action == TextEditAction::Cut)
            {
                if (tb->hasSelection())
                {
                    SDL_SetClipboardText(tb->selectedText().c_str());
                    tb->deleteSelection();
                }
            }
            else if (action == TextEditAction::Paste)
            {
                char* clip = SDL_GetClipboardText();
                if (clip)
                {
                    std::string ins = clip;
                    SDL_free(clip);
                    tb->insertText(ins);
                }
            }
            else if (action == TextEditAction::Backspace)
            {
                if (!tb->deleteSelection() && tb->cursorPos_ > 0)
                {
                    int start = detail::utf8Prev(tb->text_, tb->cursorPos_);
                    tb->text_.erase(start, tb->cursorPos_ - start);
                    tb->cursorPos_ = start;
                    tb->adjustScroll();
                    tb->markDirty();
                }
            }
            else if (action == TextEditAction::DeleteForward)
            {
                if (!tb->deleteSelection() && tb->cursorPos_ < (int)tb->text_.size())
                {
                    int clen = detail::utf8CharLen(tb->text_.c_str() + tb->cursorPos_);
                    tb->text_.erase(tb->cursorPos_, clen);
                    tb->adjustScroll();
                    tb->markDirty();
                }
            }
            else if (action == TextEditAction::ClearSelection)
            {
                tb->clearSelection();
            }
            else if (action == TextEditAction::CursorLeft ||
                     action == TextEditAction::CursorRight ||
                     action == TextEditAction::CursorHome ||
                     action == TextEditAction::CursorEnd ||
                     action == TextEditAction::SelectLeft ||
                     action == TextEditAction::SelectRight ||
                     action == TextEditAction::SelectHome ||
                     action == TextEditAction::SelectEnd)
            {
                bool select = action == TextEditAction::SelectLeft ||
                              action == TextEditAction::SelectRight ||
                              action == TextEditAction::SelectHome ||
                              action == TextEditAction::SelectEnd;
                if (select)
                {
                    if (tb->selAnchor_ < 0)
                        tb->selAnchor_ = tb->cursorPos_;
                }
                else
                {
                    tb->selAnchor_ = -1;
                }

                if ((action == TextEditAction::CursorLeft ||
                     action == TextEditAction::SelectLeft) && tb->cursorPos_ > 0)
                {
                    tb->cursorPos_ = detail::utf8Prev(tb->text_, tb->cursorPos_);
                }
                else if ((action == TextEditAction::CursorRight ||
                          action == TextEditAction::SelectRight) &&
                         tb->cursorPos_ < (int)tb->text_.size())
                {
                    tb->cursorPos_ += detail::utf8CharLen(tb->text_.c_str() + tb->cursorPos_);
                }
                else if (action == TextEditAction::CursorHome ||
                         action == TextEditAction::SelectHome)
                {
                    tb->cursorPos_ = 0;
                }
                else if (action == TextEditAction::CursorEnd ||
                         action == TextEditAction::SelectEnd)
                {
                    tb->cursorPos_ = (int)tb->text_.size();
                }

                tb->adjustScroll();
                tb->markDirty();
            }
        }

        //handle text editing for TextArea
        auto* ta = dynamic_cast<TextArea*>(focus);
        if (ta)
        {
            TextEditAction action = ta->keyMap().find(ev.key.key, ev.key.mod);

            switch (action)
            {
            case TextEditAction::CursorLeft:  ta->moveCursorLeft(false); break;
            case TextEditAction::CursorRight: ta->moveCursorRight(false); break;
            case TextEditAction::CursorUp:    ta->moveCursorUp(false); break;
            case TextEditAction::CursorDown:  ta->moveCursorDown(false); break;
            case TextEditAction::CursorHome:  ta->moveCursorHome(false); break;
            case TextEditAction::CursorEnd:   ta->moveCursorEnd(false); break;

            case TextEditAction::SelectLeft:  ta->moveCursorLeft(true); break;
            case TextEditAction::SelectRight: ta->moveCursorRight(true); break;
            case TextEditAction::SelectUp:    ta->moveCursorUp(true); break;
            case TextEditAction::SelectDown:  ta->moveCursorDown(true); break;
            case TextEditAction::SelectHome:  ta->moveCursorHome(true); break;
            case TextEditAction::SelectEnd:   ta->moveCursorEnd(true); break;

            case TextEditAction::SelectAll: ta->selectAll(); break;
            case TextEditAction::ClearSelection: ta->clearSelection(); break;

            case TextEditAction::Copy:
                if (ta->hasSelection())
                    SDL_SetClipboardText(ta->selectedText().c_str());
                break;

            case TextEditAction::Cut:
                if (ta->hasSelection() && !ta->readOnly())
                {
                    SDL_SetClipboardText(ta->selectedText().c_str());
                    ta->deleteSelection();
                }
                break;

            case TextEditAction::Paste:
                {
                    char* clip = SDL_GetClipboardText();
                    if (clip)
                    {
                        ta->insertText(clip);
                        SDL_free(clip);
                    }
                }
                break;

            case TextEditAction::Backspace:     ta->backspace(); break;
            case TextEditAction::DeleteForward: ta->deleteForward(); break;
            case TextEditAction::InsertNewline: ta->insertNewline(); break;

            default:
                break;
            }
        }

        //handle text editing for ComboBox
        auto* cb = dynamic_cast<ComboBox*>(focus);
        if (cb)
        {
            SDL_Keycode k = ev.key.key;
            bool ctrl = (ev.key.mod & SDL_KMOD_CTRL) != 0;

            if (!ctrl)
            {
                switch (k)
                {
                case SDLK_BACKSPACE: cb->backspace(); break;
                case SDLK_DELETE:    cb->deleteForward(); break;
                case SDLK_LEFT:      cb->moveCursorLeft(); break;
                case SDLK_RIGHT:     cb->moveCursorRight(); break;
                case SDLK_HOME:      cb->moveCursorHome(); break;
                case SDLK_END:       cb->moveCursorEnd(); break;
                default: break;
                }
            }
            else if (k == SDLK_V)
            {
                char* clip = SDL_GetClipboardText();
                if (clip)
                {
                    cb->insertText(clip);
                    SDL_free(clip);
                }
            }
        }

        if (focus->onKeyDown) focus->onKeyDown(ke);

        //The callback may have moved or destroyed keyboard focus.
        if (win && win->focused_ != focus)
            return;

        SDL_Keycode k = ev.key.key;

        if (auto* b = dynamic_cast<Button*>(focus))
        {
            if (k == SDLK_RETURN || k == SDLK_KP_ENTER || k == SDLK_SPACE)
            {
                MouseEvent me;
                me.x = b->width() / 2;
                me.y = b->height() / 2;
                me.screenX = b->absoluteX() + me.x;
                me.screenY = b->absoluteY() + me.y;
                me.button = SDL_BUTTON_LEFT;
                me.clicks = 1;
                if (b->onClick) b->onClick(me);
            }
        }
        else if (auto* c = dynamic_cast<CheckBox*>(focus))
        {
            if (k == SDLK_RETURN || k == SDLK_KP_ENTER || k == SDLK_SPACE)
                c->setChecked(!c->isChecked());
        }
        else if (auto* lb = dynamic_cast<ListBox*>(focus))
        {
            if (k == SDLK_UP || k == SDLK_DOWN)
            {
                int n = lb->itemCount();
                if (n > 0)
                {
                    int idx = lb->selectedIndex();
                    if (idx < 0)
                        idx = 0;
                    else if (k == SDLK_DOWN)
                        idx = std::min(idx + 1, n - 1);
                    else
                        idx = std::max(idx - 1, 0);

                    if (idx != lb->selectedIndex())
                    {
                        lb->setSelectedIndex(idx);
                        if (lb->onItemSelected) lb->onItemSelected(idx);
                    }
                }
            }
        }
        else if (auto* sl = dynamic_cast<Slider*>(focus))
        {
            float v = sl->value();
            float step = (sl->max() - sl->min()) / 100.f;
            if (step == 0.f) step = 1.f;

            if (k == SDLK_LEFT)       v -= step;
            else if (k == SDLK_RIGHT) v += step;
            else if (k == SDLK_HOME)  v = sl->min();
            else if (k == SDLK_END)   v = sl->max();

            if (v != sl->value())
            {
                sl->setValue(v);
                if (sl->onValueChanged) sl->onValueChanged(sl->value());
            }
        }
    }
    else
    {
        if (focus->onKeyUp) focus->onKeyUp(ke);
    }
}

//==============================================================================
// MessageBox
//==============================================================================

MessageBox::MessageBox()
{
}

void MessageBox::show(const char* title, const char* message,
                       MessageBoxType type, MessageBoxButtons buttons,
                       std::function<void(MessageBoxResult)> onResult,
                       int windowW, int windowH)
{
    title_    = title    ? title : "";
    message_  = message  ? message : "";
    type_     = type;
    onResult_ = std::move(onResult);
    active_   = true;
    hoveredBtn_   = -1;
    pressedBtnIdx_ = -1;

    buttons_.clear();
    auto add = [this](const char* text, MessageBoxResult r)
    {
        BtnInfo bi;
        bi.text   = text;
        bi.result = r;
        bi.rect   = {};
        bi.hovered = false;
        buttons_.push_back(bi);
    };

    switch (buttons)
    {
    case MessageBoxButtons::OK:
        add("OK", MessageBoxResult::OK); break;
    case MessageBoxButtons::OKCancel:
        add("OK", MessageBoxResult::OK);
        add("Cancel", MessageBoxResult::Cancel); break;
    case MessageBoxButtons::YesNo:
        add("Yes", MessageBoxResult::Yes);
        add("No", MessageBoxResult::No); break;
    case MessageBoxButtons::YesNoCancel:
        add("Yes", MessageBoxResult::Yes);
        add("No", MessageBoxResult::No);
        add("Cancel", MessageBoxResult::Cancel); break;
    case MessageBoxButtons::RetryCancel:
        add("Retry", MessageBoxResult::Retry);
        add("Cancel", MessageBoxResult::Cancel); break;
    }

    calcLayout(windowW, windowH);
}

void MessageBox::dismiss()
{
    active_ = false;
    onResult_ = nullptr;
    hoveredBtn_ = -1;
    pressedBtnIdx_ = -1;
}

void MessageBox::setOverlayColor(Color color) { overlayColor_ = color; }
void MessageBox::setBackgroundColor(Color color) { bgColor_ = color; }
void MessageBox::setBorderColor(Color color) { borderColor_ = color; }
void MessageBox::setTextColor(Color color) { textColor_ = color; }
void MessageBox::setTitleColors(Color background, Color text)
{
    titleBgColor_ = background;
    titleTextColor_ = text;
}
void MessageBox::setButtonColors(Color normal, Color hover, Color border, Color text)
{
    buttonBgColor_ = normal;
    buttonHoverColor_ = hover;
    buttonBorderColor_ = border;
    buttonTextColor_ = text;
}

void MessageBox::applyTheme(const Theme& theme)
{
    overlayColor_ = theme.overlay;
    bgColor_ = theme.dialogBg;
    borderColor_ = theme.dialogBorder;
    textColor_ = theme.dialogText;
    titleBgColor_ = theme.dialogTitleBg;
    titleTextColor_ = theme.dialogTitleText;
    buttonBgColor_ = theme.dialogButton;
    buttonHoverColor_ = theme.dialogButtonHover;
    buttonBorderColor_ = theme.dialogButtonBorder;
    buttonTextColor_ = theme.dialogButtonText;
}

int MessageBox::hitTestButton(int mx, int my) const
{
    if (!active_) return -1;

    if (mx < dlgX_ || mx >= dlgX_ + dlgW_ ||
        my < dlgY_ || my >= dlgY_ + dlgH_)
        return -1;

    for (int i = 0; i < (int)buttons_.size(); i++)
    {
        const auto& r = buttons_[i].rect;
        if (mx >= r.x && mx < r.x + r.w &&
            my >= r.y && my < r.y + r.h)
            return i;
    }

    return -2;
}

void MessageBox::renderOverlay(SDL_Renderer* renderer, int windowW, int windowH)
{
    if (!active_) return;

    //semi-transparent overlay
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_FRect ov = { 0, 0, (float)windowW, (float)windowH };
    SDL_SetRenderDrawColor(renderer, overlayColor_.r, overlayColor_.g,
                           overlayColor_.b, overlayColor_.a);
    SDL_RenderFillRect(renderer, &ov);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    //dialog background
    SDL_FRect dlg = { (float)dlgX_, (float)dlgY_, (float)dlgW_, (float)dlgH_ };
    SDL_SetRenderDrawColor(renderer, bgColor_.r, bgColor_.g, bgColor_.b, bgColor_.a);
    SDL_RenderFillRect(renderer, &dlg);
    SDL_SetRenderDrawColor(renderer, borderColor_.r, borderColor_.g,
                           borderColor_.b, borderColor_.a);
    SDL_RenderRect(renderer, &dlg);

    //title bar
    SDL_FRect tb = { (float)dlgX_, (float)dlgY_, (float)dlgW_, 26.0f };
    SDL_SetRenderDrawColor(renderer, titleBgColor_.r, titleBgColor_.g,
                           titleBgColor_.b, titleBgColor_.a);
    SDL_RenderFillRect(renderer, &tb);
    detail::drawText(renderer, title_.c_str(), dlgX_ + 8, dlgY_ + 5, titleTextColor_);

    //separator
    SDL_SetRenderDrawColor(renderer, borderColor_.r, borderColor_.g,
                           borderColor_.b, borderColor_.a);
    SDL_RenderLine(renderer, dlgX_, dlgY_ + 26, dlgX_ + dlgW_ - 1, dlgY_ + 26);

    //icon
    const char* icon = "";
    Color iconColor = Color::White;
    switch (type_)
    {
    case MessageBoxType::Info:     icon = "(i)"; iconColor = Color(100, 150, 255); break;
    case MessageBoxType::Warning:  icon = "(!)"; iconColor = Color(255, 200, 50);  break;
    case MessageBoxType::Error:    icon = "(X)"; iconColor = Color(255, 80, 80);   break;
    case MessageBoxType::Question: icon = "(?)"; iconColor = Color(100, 200, 100); break;
    }

    int cx = dlgX_ + 12;
    int cy = dlgY_ + 34;
    detail::drawText(renderer, icon, cx, cy, iconColor);
    int iconW = detail::textWidth(icon);

    //message text (word-wrapped)
    int msgX = cx + iconW + 10;
    int msgW = dlgW_ - 24 - iconW - 10;
    if (msgW < 20) msgW = 20;

    std::vector<std::string> lines;
    detail::wrapText(message_.c_str(), msgW, lines);

    int lineH = currentFont()->lineHeight();
    int curY = cy;
    for (const auto& line : lines)
    {
        if (!line.empty())
            detail::drawText(renderer, line.c_str(), msgX, curY, textColor_);
        curY += lineH;
    }

    //buttons — use pre-calculated rects from calcLayout
    for (int i = 0; i < (int)buttons_.size(); i++)
    {
        auto& bi = buttons_[i];
        const SDL_FRect& r = bi.rect;
 
        bool hover = (i == hoveredBtn_);
        Color bbg = hover ? buttonHoverColor_ : buttonBgColor_;
        SDL_SetRenderDrawColor(renderer, bbg.r, bbg.g, bbg.b, bbg.a);
        SDL_RenderFillRect(renderer, &r);
        SDL_SetRenderDrawColor(renderer, buttonBorderColor_.r, buttonBorderColor_.g,
                               buttonBorderColor_.b, buttonBorderColor_.a);
        SDL_RenderRect(renderer, &r);

        int tx = r.x + (r.w - detail::textWidth(bi.text.c_str())) / 2;
        int ty = r.y + (int)((r.h - currentFont()->lineHeight()) / 2);
        detail::drawText(renderer, bi.text.c_str(), tx, ty, buttonTextColor_);
    }
}

void MessageBox::calcLayout(int windowW, int windowH)
{
    //determine dlgW_ first (independent of message width)
    int btnTotalW = 0;
    std::vector<int> btnWidths;
    for (auto& bi : buttons_)
    {
        int bw = detail::textWidth(bi.text.c_str()) + 20;
        btnWidths.push_back(bw);
        btnTotalW += bw;
    }
    btnTotalW += ((int)buttons_.size() - 1) * 10;

    int titleW = detail::textWidth(title_.c_str());
    int reqW = std::max({ titleW + 20, btnTotalW + 24, 280 });
    dlgW_ = std::min(420, reqW);
    if (dlgW_ < 200) dlgW_ = 200;

    //now calc message line count using actual available width
    int iconW = 24;
    int availW = dlgW_ - 24 - iconW - 10;
    if (availW < 20) availW = 20;

    std::vector<std::string> lines;
    detail::wrapText(message_.c_str(), availW, lines);
    int lineH = std::max(1, currentFont()->lineHeight());
    int msgLines = (int)lines.size();
    int msgH = msgLines * lineH;
    if (msgH < lineH) msgH = lineH;

    int contentH = std::max(lineH, msgH);
    dlgH_ = 26 + 8 + contentH + 12 + 26 + 6;

    dlgX_ = (windowW - dlgW_) / 2;
    dlgY_ = (windowH - dlgH_) / 2;

    //pre-calculate button rects
    int btnGap = 10;
    int btnStartX = dlgX_ + (dlgW_ - btnTotalW) / 2;
    int btnY = dlgY_ + dlgH_ - 36;
    for (int i = 0; i < (int)buttons_.size(); i++)
    {
        buttons_[i].rect = { (float)btnStartX, (float)btnY, (float)btnWidths[i], 26.0f };
        btnStartX += btnWidths[i] + btnGap;
    }
}

#endif //SDLGUI_IMPLEMENTATION
} //namespace sdlgui
#endif //SDLGUI_HPP
