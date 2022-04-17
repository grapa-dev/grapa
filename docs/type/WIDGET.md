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
```

#### "double_window"
[FTLK documentatin - Fl_Double_Window](https://www.fltk.org/doc-1.3/classFl__Double__Window.html)

Dobule buffering version of "window", so updates are smother. 

```
w = $WIDGET("double_window", 0, 0, 640, 320, "test", {color: "BLUE"});
w.show();
```

#### "hor_nice_slider"
[FTLK documentatin - FL_Hor_Nice_Slider](https://www.fltk.org/doc-1.3/classFl__Hor__Nice__Slider.html)

```
w = $WIDGET("double_window", 0, 0, 640, 320, "test", {color: "BLUE"});
w.show();
w += (ns:$WIDGET("hor_nice_slider", 20, 20, 600, 20, "", {scrollvalue:[30,5,0,100]}));
```

The above can also be initiated using the following.
```
w = $WIDGET("double_window", 0, 0, 640, 320, "test", {
  color: "BLUE",
  children: {
    ns:$WIDGET("hor_nice_slider", 20, 20, 600, 20, "", {
      scrollvalue:[30,5,0,100]
      })
    }
  });
w.show();
```


#### "scrollbar"
[FTLK documentatin - FL_Scrollbar](https://www.fltk.org/doc-1.3/classFl__Scrollbar.html)

```
w = $WIDGET("double_window", 0, 0, 640, 320, "test", {color: "BLUE"});
w.show();
w += (ns:$WIDGET("scrollbar", 600, 20, 20, 280, "", {scrollvalue:[30,5,0,100]}));
```

#### "scroll"
[FTLK documentatin - FL_Scroll](https://www.fltk.org/doc-1.3/classFl__Scroll.html)

```
w = $WIDGET("double_window", 0, 0, 340, 220, "test", {color: "BLUE"});
w.show();
w += (ns:$WIDGET("scroll", 20, 20, 300, 180));
w.child("ns") += (tx:$WIDGET("text_display", 20, 20, 800, 340));
w.child("ns").child("tx").set({text:$sys().getenv('LICENCE')});
w.redraw();
```

#### "button"
[FTLK documentatin - FL_Button](https://www.fltk.org/doc-1.3/classFl__Button.html)

```
w = $WIDGET("double_window", 0, 0, 340, 220, "test", {color: "BLUE"});
w.show();
w += (b1:$WIDGET("button", 20, 20, 60, 20, "B1"));
w += (b2:$WIDGET("button", 20, 40, 60, 20, "B2"));
```

#### "radio_button"
[FTLK documentatin - FL_Radio_Button](https://www.fltk.org/doc-1.3/classFl__Radio__Button.html)

```
w = $WIDGET("double_window", 0, 0, 340, 220, "test", {color: "BLUE"});
w.show();
w += (b1:$WIDGET("radio_button", 20, 20, 60, 20, "B1"));
w += (b2:$WIDGET("radio_button", 20, 40, 60, 20, "B2"));
```

#### "toggle_button"
[FTLK documentatin - FL_Toggle_Button](https://www.fltk.org/doc-1.3/classFl__Toggle__Button.html)

```
w = $WIDGET("double_window", 0, 0, 340, 220, "test", {color: "BLUE"});
w.show();
w += (b1:$WIDGET("toggle_button", 20, 20, 60, 20, "B1"));
w += (b2:$WIDGET("toggle_button", 20, 40, 60, 20, "B2"));
```

#### "group"
[FTLK documentatin - FL_Group](https://www.fltk.org/doc-1.3/classFl__Group.html)

```
w = $WIDGET("double_window", 0, 0, 340, 220, "test", {color: "BLUE"});
w.show();
w += (ns:$WIDGET("group", 20, 20, 40, 60));
w.child("ns") +=  (b1:$WIDGET("button", 20, 20, 60, 20, "B1"));
w.child("ns") +=  (b2:$WIDGET("button", 20, 40, 60, 20, "B2"));
```

After the above, run the following to move the group.

```
r = w.child("ns").get(["x","y","w","h"]);
w.child("ns").resize(r.x+10,r.y+10,r.w,r.h);
```

#### "pack"
[FTLK documentatin - FL_Pack](https://www.fltk.org/doc-1.3/classFl__Pack.html)

```
w = $WIDGET("double_window", 0, 0, 340, 220, "test", {color: "BLUE"});
w.show();
w += (ns:$WIDGET("pack", 20, 20, 40, 60, "", {spacing: 4}));
w.child("ns") +=  (b1:$WIDGET("button", 20, 20, 60, 20, "B1"));
w.child("ns") +=  (b2:$WIDGET("button", 20, 40, 60, 20, "B2"));
```

Horizontal version.

```
w = $WIDGET("double_window", 0, 0, 340, 220, "test", {color: "BLUE"});
w.show();
w += (ns:$WIDGET("pack", 20, 20, 0, 20, "", {type: "horizontal", spacing: 4}));
w.child("ns") +=  (b1:$WIDGET("button", 20, 20, 60, 20, "B1"));
w.child("ns") +=  (b2:$WIDGET("button", 20, 40, 60, 20, "B2"));
```

#### "text_editor"
[FTLK documentatin - FL_Text_Editor](https://www.fltk.org/doc-1.3/classFl__Text__Editor.html)

```
w = $WIDGET("double_window", 0, 0, 340, 220, "test", {color: "BLUE"});
w.show();
w += (ns:$WIDGET("scroll", 20, 20, 300, 180));
w.child("ns") +=  (tx:$WIDGET("text_editor", 20, 20, 800, 340));
w.child("ns").child("tx").set({text:$sys().getenv('LICENCE')});
w.set({resizable: "ns"});
w.redraw();
```

#### "text_display"
[FTLK documentatin - FL_Text_Display](https://www.fltk.org/doc-1.3/classFl__Text__Display.html)

```
w = $WIDGET("double_window", 0, 0, 340, 220, "test", {color: "BLUE"});
w.show();
w += (ns:$WIDGET("scroll", 20, 20, 300, 180));
w.child("ns") +=  (tx:$WIDGET("text_display", 20, 20, 800, 340));
w.child("ns").child("tx").set({text:$sys().getenv('LICENCE')});
w.set({resizable: "ns"});
w.redraw();
```

#### "menu_bar"
[FTLK documentatin - FL_Menu_Bar](https://www.fltk.org/doc-1.3/classFl__Menu__Bar.html)

```
w = $WIDGET("double_window", 20, 50, 340, 220, "test", {color: "BLUE"});
w.show();
w +=  (tx:$WIDGET("text_display", 20, 50, 300, 150));
w.child("tx").set({text:"blank"});

w += (menu: $WIDGET("menu_bar", 0, 0, 640, 30));
m1_cb = op(o,cbdata,item) {o.parent().child("tx").set({text:"M1"});};
w.child("menu") += (M1: {path: "&File/&M1", flags: ["DIVIDER"], shortcut: "^a", callback: m1_cb});
m2_cb = op(o,cbdata,item) {o.parent().child("tx").set({text:"M2"});};
w.child("menu") += (M2: {path: "&File/&M2", shortcut: "^b", callback: m2_cb});
```

#### "widget"
[FTLK documentatin - FL_Widget](https://www.fltk.org/doc-1.3/classFl__Widget.html)

Widget that does nothing. Useful for inserting a spacer in a "pack". 

```
w = $WIDGET("double_window", 0, 0, 340, 220, "test", {color: "BLUE"});
w.show();
w += (ns:$WIDGET("pack", 20, 20, 0, 20, "", {type: "horizontal", spacing: 4}));
w.child("ns") +=  (b1:$WIDGET("button", 20, 20, 60, 20, "B1"));
w.child("ns") +=  (b2:$WIDGET("button", 20, 40, 60, 20, "B2"));
w.child("ns") +=  (s1:$WIDGET("widget", 20, 40, 20, 20));
w.child("ns") +=  (b3:$WIDGET("button", 20, 40, 60, 20, "B3"));
w.child("ns") +=  (b4:$WIDGET("button", 20, 40, 60, 20, "B4"));
```

#### "table_row"
[FTLK documentatin - FL_Table_Row](https://www.fltk.org/doc-1.3/classFl__Table__Row.html)

```
w = $WIDGET("double_window", 20, 50, 440, 340, "test", {color: "BLUE"});
w.show();
w += (table: $WIDGET("table_row", 20, 20, 400, 300));
w.child("table").set({vector:$file().ls().vector()});
```

### label
A label for widgets that use a lable, such as "window".

### attrlist
Calls set(attrlist)

## show ()
Shows a widget.

```
w = $WIDGET("double_window", 0, 0, 340, 220, "test", {color: "BLUE"});
w.show();
```

## hide ()
Hides a widget.

```
w = $WIDGET("double_window", 0, 0, 340, 220, "test", {color: "BLUE"});
w.show();
w += (b1:$WIDGET("button", 20, 20, 60, 20, "B1"));
w += (b2:$WIDGET("button", 20, 40, 60, 20, "B2"));
w.child("b1").hide();
```

## redraw ()
[FTLK documentatin - redraw()](https://www.fltk.org/doc-1.3/classFl__Widget.html#aa63ce68cbf4620cf8750b868368ea02b)

A message can be sent to a widget to request a redraw, for situations where a change is made but the widget does not automatically update to reflect the change.

```
w = $WIDGET("double_window", 0, 0, 340, 220, "test", {color: "BLUE"});
w.show();
w += (b1:$WIDGET("button", 20, 20, 60, 20, "B1"));
w += (b2:$WIDGET("button", 20, 40, 60, 20, "B2"));
w.child("b1").redraw();
```

## resizable (resizewidget)
[FTLK documentatin - resizable()](https://www.fltk.org/doc-1.3/classFl__Group.html#afd220e45e1ac817bde7d6a25fdf74e37)

Binds the resizable definition for a widget to another widget. For example, a window may have both a text box and a button and resizing the window should result in resizing the text box but not the button. In this case, the resizable definition for the window should include the text box, but not the button. 

```
w = $WIDGET("double_window", 0, 0, 340, 220, "test", {color: "BLUE"});
w.show();
w.resizable(w);

w += (ns:$WIDGET("pack", 20, 20, 300, 180, "", {spacing: 4}));

w.child("ns") += (bt:$WIDGET("pack", 20, 20, 0, 20, "", {type: "horizontal", spacing: 4}));
w.child("ns").child("bt") +=  (bt1:$WIDGET("button", 20, 20, 40, 20, "btn1"));
w.child("ns").child("bt") +=  (bt2:$WIDGET("button", 20, 20, 40, 20, "btn2"));

w.child("ns") +=  (tx:$WIDGET("text_display", 20, 40, 300, 160));
w.resizable(w.child("ns"));
w.child("ns").resizable(w.child("ns").child("tx"));

w.child("ns").child("tx").set({text:$sys().getenv('LICENCE')});
```


## resize (x, y, w, h)
[FTLK documentatin - resize()](https://www.fltk.org/doc-1.3/classFl__Widget.html#aca98267e7a9b94f699ebd27d9f59e8bb)

```
w = $WIDGET("double_window", 0, 0, 340, 220, "test", {color: "BLUE"});
w.show();
w += (ns:$WIDGET("scroll", 20, 20, 300, 180));
w.child("ns") +=  (tx:$WIDGET("text_display", 20, 20, 800, 340));
w.child("ns").child("tx").set({text:$sys().getenv('LICENCE')});
w.set({resizable: "ns"});
w.redraw();

r = w.get(["x","y","w","h"]);
w.resize(r.x,r.y,r.w+50,r.h+100);
```

## parent ()
[FTLK documentatin - parent()](https://www.fltk.org/doc-1.3/classFl__Widget.html#a6f55261683ecaf68d70e424213d030b0)

Parent widget...which will always be a "group" type of widget. Useful in handlers that get passed the widget for the handler, and the script needs to make a change to another widget that is relative to the target widget. An example would be a button that updates a text display. 

```
w = $WIDGET("double_window", 0, 0, 340, 260, "test", {color: "BLUE"});
w.show();
w += (ns:$WIDGET("button", 20, 20, 40, 20, "time"));
w += (tx:$WIDGET("text_display", 20, 40, 300, 200));
w.child("ns").set({when_release: op(o) {o.parent().child("tx").set({text:($TIME().utc()+$TIME().tz()).str()});}}));
```

## child (name)
[FTLK documentatin - child()](https://www.fltk.org/doc-1.3/classFl__Group.html#ab564b4b536ad8eb62885523220348d1f)

Child of widget. Name can either be a string or a number. If number, than it is the n'th widget in the group.

```
w = $WIDGET("double_window", 0, 0, 340, 260, "test", {color: "BLUE"});
w.show();
w += (ns:$WIDGET("button", 20, 20, 40, 20, "time"));
w += (tx:$WIDGET("text_display", 20, 40, 300, 200));
w.child("ns").set({when_release: op(o) {o.parent().child("tx").set({text:($TIME().utc()+$TIME().tz()).str()});}}));
```

## next ()
Next widget in the group. This is useful when an action with a widget in a group needs to modify the widget that follows in the group.

```
w = $WIDGET("double_window", 0, 0, 340, 260, "test", {color: "BLUE"});
w.show();
w += (ns:$WIDGET("button", 20, 20, 40, 20, "time"));
w += (tx:$WIDGET("text_display", 20, 40, 300, 200));
w.child("ns").set({when_release: op(o) {o.next().set({text:($TIME().utc()+$TIME().tz()).str()});}}));
```

## focus ()
[FTLK documentatin - focus()](https://www.fltk.org/doc-1.3/group__fl__events.html#ga7f0e9836db43268979e0b3a999583b7f)

Get's the current widget in focus.

The following example has a menu item that will update the in focus text editor with the current time.

```
w = $WIDGET("double_window", 20, 50, 340, 220, "test", {color: "BLUE"});
w.show();
w +=  (tx:$WIDGET("text_editor", 20, 50, 300, 75));
w +=  (tx:$WIDGET("text_editor", 20, 125, 300, 75));

w += (menu: $WIDGET("menu_bar", 0, 0, 640, 30));
m1_cb = op(o,cbdata,item) {o.focus().set({text:($TIME().utc()+$TIME().tz()).str()});};
w.child("menu") += (Time: {path: "&File/&Time", shortcut: "^a", callback: m1_cb});
```

## get (attr_array)

### widget attributes

name | desc
------------ | -------------
"name" | 
"visible" | 
"x" | 
"y" | 
"w" | 
"h" | 
"size" |
"label" |

### scrollbar attributes

name | desc
------------ | -------------
"linesize" |
"scrollvalue" |

### text_display, text_editor attributes

name | desc
------------ | -------------
"mNBufferLines" |
"mMaxsize" |
"text" |
"selection" |
"start" |
"end" |
"row" |
"col" |

### table_row attributes

name | desc
------------ | -------------
"rows" |
"cols" |
"shape" |

### menu_bar attributes
The array item is a list, where for each item the label is menu item label and the item value is an array of attributes to query for that menu item.

name | desc
------------ | -------------
"shortcut" |
"options" |
"labelsize" |
"data" |
"callback" |
"name" |
"label" |
"path" |

```
w = $WIDGET("double_window", 20, 50, 340, 220, "test", {color: "BLUE"});
w.show();
w +=  (tx:$WIDGET("text_display", 20, 50, 300, 150));
w.child("tx").set({text:"blank"});

w += (menu: $WIDGET("menu_bar", 0, 0, 640, 30));
m1_cb = op(o,cbdata,item) {o.parent().child("tx").set({text:"M1"});};
w.child("menu") += (M1: {path: "&File/&M1", flags: ["DIVIDER"], shortcut: "^a", callback: m1_cb});
m2_cb = op(o,cbdata,item) {o.parent().child("tx").set({text:"M2"});};
w.child("menu") += (M2: {path: "&File/&M2", shortcut: "^b", callback: m2_cb});

w.child("menu").get([ {"M1":["path","shortcut"]} ]);
```

## set (attr_list)

### widget attributes

name | value | desc
------------ | ------------- | -------------
"namespace" | "global" | Variable resolution connected directly with global rather than parent widget.
"x" | 
"y" | 
"w" | 
"h" | 
"size" | 
"scroll_type" |  "HORIZONTAL", "VERTICAL"
"orientation" | "HORIZONTAL", "VERTICAL"
"jpeg" | 
"label" | 
"labelfont" | 
"resizable" | 
"color" | 

### handler attributes
name | value | desc
------------ | ------------- | -------------
"on_post_start" | $OP |
"on_post_prompt" | $OP |
"on_post_end" | $OP |
"on_post_echo" | $OP |
"on_post_message" | $OP |
"on_hide" | $OP |

### scroll attributes

name | desc
------------ | -------------
"scroll_type" |  "HORIZONTAL", "VERTICAL", "BOTH", "ALWAYS_ON", "HORIZONTAL_ALWAYS", "VERTICAL_ALWAYS", "BOTH_ALWAYS"

### scrollbar attributes

name | value
------------ | -------------
"on_drag" | $OP |
"on_push" | $OP |
"on_release" | $OP |
"linesize" | 
"scrollvalue":[pos,size,first,total] | 

### hor_nice_slider attributes

name | value
------------ | -------------
"scrollvalue":[pos,size,first,total] | 

### button attributes
name | value
------------ | -------------
"on_release" | $OP |

### group attributes

name | value
------------ | -------------
"child" | list of {name: list of widget initialization attributes}
"child" | list of {name: $WIDGET}

```
w = $WIDGET("double_window", 0, 0, 340, 220, "test", {color: "BLUE"});
w.show();
w += (b1:$WIDGET("button", 20, 20, 60, 20, "B1"));
w += (b2:$WIDGET("button", 20, 40, 60, 20, "B2"));

w.set({child:{b3:["button", 20, 60, 60, 20, "B3"]}});
w.child("b3").redraw();

w.set({child:{b4:$WIDGET("button", 20, 80, 60, 20, "B4")}});
w.child("b4").redraw();
```

### menu_bar attributes

name | value
------------ | -------------
"child" | list of {name: list of attributes}

name | value | desc
------------ | ------------- | -------------
"name" |  | 
"path" |  | 
"data" |  | 
"label" |  | 
"flags" |  | 
"labelfont" |  | 
"labelsize" |  | 
"labelcolor" |  | 
"setonly" |  | 
"shortcut" |  | 
"callback" |  $OP | 

```
w = $WIDGET("double_window", 20, 50, 340, 220, "test", {color: "BLUE"});
w.show();
w +=  (tx:$WIDGET("text_display", 20, 50, 300, 150));
w.child("tx").set({text:"blank"});
w += (menu: $WIDGET("menu_bar", 0, 0, 640, 30));
m1_cb = op(o,cbdata,item) {o.parent().child("tx").set({text:"M1"});};
w.child("menu") += (M1: {path: "&File/&M1", flags: ["DIVIDER"], shortcut: "^a", callback: m1_cb});
m2_cb = op(o,cbdata,item) {o.parent().child("tx").set({text:"M2"});};
w.child("menu") += (M2: {path: "&File/&M2", flags: ["DIVIDER"], shortcut: "^a", callback: m2_cb});

w.child("menu").set({child:{M1: {label: "M1X", "labelsize":18}}});

```

### text_display and text_editor attributes

name | desc
------------ | -------------
"text" | 
"append" | 
"wrap" | "NONE", "COLUMN", "PIXEL", "BOUNDS"
"key" | 
"textsize" | 
"textfont" | 
"textcolor" | 
"cursor_state" | 
"cursor_color" | 
"cursor_style" | 
"cursor_visible" | 
"on_resize" | $OP |
"on_keydown" | $OP |
"on_mousewheel" | $OP |
"on_drag" | $OP |
"on_push" | $OP |
"on_release" | $OP |

### pack attributes

name | desc
------------ | -------------
"spacing" | 

### table_row attributes

name | desc
------------ | -------------
"shape" | 
"vector" | 
"on_draw_cell" | $OP |

### other attributes

name | desc
------------ | -------------
"get" | attr_array
"exec" | $OP | w.set({exec:op(){5.3*4.6}})

## handle (event)

## event_key (key)

## append (data)

## post (postop, postparams, doneop)

## clear ()
