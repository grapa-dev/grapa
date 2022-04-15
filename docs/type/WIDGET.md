# $WIDGET

To try test scripts, run grapa in with the editor screen by either using "grapa -w" from the command line, or if in grapa console, enter "$editor()" and then ">". The grapa source for the editor is here [$editor.grc](../../lib/grapa/$editor.grc).

## $new (widget, x, y, w, h, label, attrlist)
The $WIDGET type includes an initialization function $new that is called when the widget object is created, passing in the widget type name, the screen boundaries, an optional label, and an optional set of attributes. The design is an implementation of the FTLK library, and so references included to the related library documentation. 

When a widget is created, an optional attribute list can be provided. See the Set command for what is supported for each widget. 

To delete a widget, assign null to the variable holding the widget.

If a variable holding a widget is assigned to another variable, both variables will point to the same widget. Take caution when doing this...or avoid doing this. 

### widget

#### "window"
[FTLK documentatin - Fl_Window](https://www.fltk.org/doc-1.3/classFl__Window.html)

```
  w = $WIDGET("window", 0, 0, 640, 320, "test", {color: "BLUE"});
  w.show();
  w = null;
```

#### "double_window"
[FTLK documentatin - Fl_Double_Window](https://www.fltk.org/doc-1.3/classFl__Double__Window.html)

Dobule buffering version of "window", so updates are smother. 

```
  w = $WIDGET("double_window", 0, 0, 640, 320, "test", {color: "BLUE"});
  w.show();
  w = null;
```

#### "hor_nice_slider"
[FTLK documentatin - FL_Hor_Nice_Slider](https://www.fltk.org/doc-1.3/classFl__Hor__Nice__Slider.html)


#### "scrollbar"

#### "scroll"

#### "resizable_scroll"

#### "button"

#### "radio_button"

#### "toggle_button"

#### "text_editor"

#### "text_display"

#### "group"

#### "pack"

#### "menu_bar"

#### "widget"

#### "table_row"

### label

### attrlist
Calls set(attrlist)

## show ()

## hide ()

## redraw ()

## resizable (widget)

## resize (x, y, w, h)

## parent ()

## child (name)

## next ()

## focus ()

## get (name, params) or (attrlist)

## set (name, data) or (attrlist)

name | desc
------------ | -------------
"namespace" | 
"on_post_start" | 
"on_post_prompt" | 
"on_post_end" | 
"on_post_echo" | 
"on_post_message" | 
"children" | 
"on_hide" | 
"resizable" | 
"text" | 
"append" | 
"spacing" | 
"wrap" | 
"x" | 
"y" | 
"w" | 
"h" | 
"size" | 
"shape" | 
"vector" | 
"key" | 
"color" | 
"textsize" | 
"menu_bar" | 
"textfont" | 
"textcolor" | 
"cursor_state" | 
"cursor_color" | 
"cursor_style" | 
"scroll_type" | 
"orientation" | 
"on_resize" | 
"on_keydown" | 
"on_mousewheel" | 
"when_release" | 
"on_drag" | 
"on_push" | 
"on_release" | 
"on_draw_cell" | 
"jpeg" | 
"linesize" | 
"scrollvalue" | 
"label" | 
"labelfont" | 
"get" | 
"exec" | 


## handle (event)

## event_key (key)

## append (data)

## post (postop, postparams, doneop)

## clear ()
