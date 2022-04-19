# $WIDGET

name | scope | get | set
------------ | ------------- | ------------- | -------------
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

name | scope | get | set
------------ | ------------- | ------------- | -------------
"namespace" | widget | X | :heavy_check_mark:
"name" | widget
"visible" | widget
"x" | widget
"y" | widget
"w" | widget
"h" | widget
"size" | widget
"label" | widget
"linesize" | scrollbar
"scrollvalue" | scrollbar
"mNBufferLines" | text
"mMaxsize" | text
"text" | text
"selection" | text
"start" | text
"end" | text
"row" | text
"col" | text
"rows" | table_row
"cols" | table_row
"shape" | table_row
"shortcut" | menu_bar
"options" | menu_bar
"labelsize" | menu_bar
"data" | menu_bar
"callback" | menu_bar
"name" | menu_bar
"label" | menu_bar
"path" | menu_bar

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

### text attributes

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

### window and double_window attributes
name | value | desc
------------ | ------------- | -------------
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
w += (tx:$WIDGET("text_display", 20, 50, 300, 150));
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

### on_post handlers
Enables sending scripts (raw text script, not an $OP) to a widget to be compiled and run in a thread specific to that widget (async to other threads). 

name | value | desc
------------ | ------------- | -------------
"on_post_start" | $OP |
"on_post_end" | $OP |
"on_post_echo" | $OP |

The script below will setup on_post handlers in the text_display widget, and then will exec a function in that widget to "post" a text script to be compiled and run in a thread. 

```
w = $WIDGET("double_window", 0, 0, 340, 260, "test", {color: "BLUE"});
w.show();
w += (ns:$WIDGET("button", 20, 20, 40, 20, "clear", {on_release: op(o){o.next().set({"text":""});} }));
w += (tx:$WIDGET("text_display", 20, 40, 300, 200));
w.child("tx").set({  
	on_post_start: op(o)
  	{
		o.set({text:""});
		o.append("on_post_start\n");
	},
	on_post_echo: op(o,data)
	{
		o.append("on_post_echo:"+data.str()+"\n");
	},
	on_post_end: op(o,data)
	{
		o.append("on_post_end:"+data.str()+"\n");
	}
   });

w.child("tx").set({exec:op(){"(1).echo();2;".post();}});
```

The result of the sccript below will create a window with a button and a text_display, with the text_display set to:
```
on_post_start
on_post_echo:1
on_post_end:2
```


### other attributes

name | type | desc
------------ | ------------- | -------------
"get" | attr_array
"exec" | $OP | w.child("tx").set({exec:op(){$this.get(["text"]);}});

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
