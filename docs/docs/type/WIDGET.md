# $WIDGET


To try test scripts, run grapa in with the editor screen by either using "grapa -w" from the command line, or if in grapa console, enter ">".

To run grapa from the command line with a custom widget script, use "grapa -wfile filename". To speed up load time, compile the script using $sys().compile(infile, outfile). 

The $WIDGET type includes an initialization method $new that is called when the widget object is created, passing in the widget type name, the screen boundaries, an optional label, and an optional set of attributes. The design is an implementation of the FTLK library, and so references included to the related library documentation. 

All widgets are created using the following syntax, where label and attrlist are optional.
- widgetname = name of the widget, such as "window" or "button"
- x, y, w, h = coordinates and size of the widget
- label = title for the widget
- attrlist = initialization properties, wich could include definitions for child widgets and event handlers

```
w = $WIDGET(widgetname, x, y, w, h, label, attrlist)
```

To delete a widget, assign null to the variable holding the widget.

If a variable holding a widget is assigned to another variable, both variables will point to the same widget. Take caution when doing this...or avoid doing this. 

Several widgets support a group of widgets. There are 3 equivalent ways to create and add a widget to an existing one.
```
w = $WIDGET("double_window", 0, 0, 640, 320, "test");
w += (b1:$WIDGET("button", 20, 20, 60, 20, "B1"));
w.set({child:{b2:["button", 20, 40, 60, 20, "B2"]}});
w.set({child:{b3:$WIDGET("button", 20, 50, 60, 20, "B3")}});
```

See [$WIDGET attributes](widget/attributes.md) for a list of attributes that can be get or set for each widget type. 

## widgetname

### "window"
[FTLK documentatin - Fl_Window](https://www.fltk.org/doc-1.3/classFl__Window.html)

```
w = $WIDGET("window", 0, 0, 640, 320, "test", {color: "BLUE"});
w.show();
```

### "double_window"
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

w.child("menu").get([ {"M1":["path","shortcut"]} ]);
w.child("menu").get([ {"M1":["label","labelsize"]} ]);
w.child("menu").set({child:{M1: {label: "M1X", "labelsize":18}}});
w.child("menu").get([ {"M1":["label","labelsize"]} ]);

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
w.child("ns").set({on_release: op(o) {o.parent().child("tx").set({text:($TIME().utc()+$TIME().tz()).str()});}});
```

## child (name)
[FTLK documentatin - child()](https://www.fltk.org/doc-1.3/classFl__Group.html#ab564b4b536ad8eb62885523220348d1f)

Child of widget. Name can either be a string or a number. If number, than it is the n'th widget in the group.

```
w = $WIDGET("double_window", 0, 0, 340, 260, "test", {color: "BLUE"});
w.show();
w += (ns:$WIDGET("button", 20, 20, 40, 20, "time"));
w += (tx:$WIDGET("text_display", 20, 40, 300, 200));
w.child("ns").set({on_release: op(o) {o.parent().child("tx").set({text:($TIME().utc()+$TIME().tz()).str()});}});
```

## next ()
Next widget in the group. This is useful when an action with a widget in a group needs to modify the widget that follows in the group.

```
w = $WIDGET("double_window", 0, 0, 340, 260, "test", {color: "BLUE"});
w.show();
w += (ns:$WIDGET("button", 20, 20, 40, 20, "time"));
w += (tx:$WIDGET("text_display", 20, 40, 300, 200));
w.child("ns").set({on_release: op(o) {o.next().set({text:($TIME().utc()+$TIME().tz()).str()});}});
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
Gets a list of attributes as specified.

See [$WIDGET attributes](widget/attributes.md).

## set (attr_list)
Sets a list of attributes as specified.

See [$WIDGET attributes](widget/attributes.md).

## handle (event)
[FTLK documentatin - handle()](https://www.fltk.org/doc-1.3/classFl__Widget.html#a9cb17cc092697dfd05a3fab55856d218)

In an event handler, such as "on_keydown", if the event is not processed by the "on_keydown" function, call handle(event).

The following will process a text selection when Shift+Enter is pressed. If not pressed, than the default event handler is called for the widget.

```
on_keydown: op(o,event)
{
  $local.handled = 0;
  if (((o.event_key() & (0x7f).int()) == 0x0d) && (o.event_key((0xffe1).int())))
  {
    $local.s = o.get("selection");
    if (s==""||s.type()=="$ERR")
      s = o.get("text");
    if (s!="")
    {
      $local.t = s.post();
    };
    handled = 1;
  }
  else
  {
    handled = o.handle(event);
  };
  handled;
}
```

## event_key (key)
[FTLK documentatin - event_key()](https://www.fltk.org/doc-1.3/group__fl__events.html#ga1ac131e3cd5ca674cc022b1f77233449)

Returns the key lasted pressed if key parameter not passed in. If key parameter passed in, returns true if key is pressed.

See "handle" example.

## append (data)
Appends to text_display or text_editor widget.

```
w = $WIDGET("double_window", 20, 50, 340, 220, "test", {color: "BLUE"});
w.show();
w += (tx:$WIDGET("text_display", 20, 50, 300, 150));
w.child("tx").set({text:"blank"});

w.child("tx").append("hi");
```

## post (postop, postparams, doneop)
Runs postop in the widget thread with postparams. Runs doneop when done.

```
w = $WIDGET("double_window", 0, 0, 340, 260, "test", {color: "BLUE"});
w.show();
w += (ns:$WIDGET("button", 20, 20, 40, 20, "clear", {on_release: op(o){o.next().set({"text":""});} }));
w += (tx:$WIDGET("text_display", 20, 40, 300, 200));
w.child("tx").post(op(p){$self.set({text:p+" start\n"})},"test",op(p){$self.set({append:p+" end\n"})});
```

## clear ()
Removes all child elements from menu or group widget. 
