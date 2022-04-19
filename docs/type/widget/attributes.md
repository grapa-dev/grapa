# $WIDGET attributes

scope | name | get | set | options
------------ | ------------- | ------------- | ------------- | -------------
widget | "namespace" | :heavy_minus_sign: | :heavy_check_mark: | If set, "global" is the only option.
widget | "name" | :heavy_check_mark: | :heavy_minus_sign: |
widget | "visible" | :heavy_check_mark: | :heavy_minus_sign: |
widget | "x" | :heavy_check_mark: | :heavy_check_mark: |
widget | "y" | :heavy_check_mark: | :heavy_check_mark: |
widget | "w" | :heavy_check_mark: | :heavy_check_mark: |
widget | "h" | :heavy_check_mark: | :heavy_check_mark: |
widget | "size" | :heavy_check_mark: | :heavy_check_mark: |
widget | "label" | :heavy_check_mark: | :heavy_check_mark: |
widget | "scroll_type" | :heavy_minus_sign: | :heavy_check_mark: |  "HORIZONTAL", "VERTICAL"
widget | "orientation" | :heavy_minus_sign: | :heavy_check_mark: | "HORIZONTAL", "VERTICAL"
widget | "jpeg" | :heavy_minus_sign: | :heavy_check_mark: | 
widget | "labelfont" | :heavy_minus_sign: | :heavy_check_mark: | 
widget | "resizable" | :heavy_minus_sign: | :heavy_check_mark: | 
widget | "color" | :heavy_minus_sign: | :heavy_check_mark: |
widget | "on_hide" | :heavy_minus_sign: | :heavy_check_mark: | $OP
widget | "get" | :heavy_minus_sign: | :heavy_check_mark: | attr_array - there are some instances where it is useful to get the value off some attributes right after setting something.
widget | "exec" | :heavy_minus_sign: | :heavy_check_mark: | $OP - runs an $OP within a widget. $self will contain the reference to the widget. Example: w.child("tx").set({exec:op(){$this.get(["text"]);}}); If the goal is to run the $OP in the background in the widget, use the post method.
widget | "on_post_start" | :heavy_minus_sign: | :heavy_check_mark: | $OP - used only in $editor.grc
widget | "on_post_end" | :heavy_minus_sign: | :heavy_check_mark: | $OP - used only in $editor.grc
widget | "on_post_echo" | :heavy_minus_sign: | :heavy_check_mark: | $OP - used only in $editor.grc
scroll | "scroll_type" | :heavy_minus_sign: | :heavy_check_mark: |   "HORIZONTAL", "VERTICAL", "BOTH", "ALWAYS_ON", "HORIZONTAL_ALWAYS", "VERTICAL_ALWAYS", "BOTH_ALWAYS"
scrollbar | "linesize" | :heavy_check_mark: | :heavy_check_mark: |
scrollbar | "scrollvalue" | :heavy_check_mark: | :heavy_check_mark: | [pos,size,first,total]
scrollbar | "on_drag" | :heavy_minus_sign: | :heavy_check_mark: | $OP
scrollbar | "on_push" | :heavy_minus_sign: | :heavy_check_mark: | $OP
scrollbar | "on_release" | :heavy_minus_sign: | :heavy_check_mark: | $OP
hor_nice_slider | "scrollvalue" | :heavy_check_mark: | :heavy_check_mark: | [pos,size,first,total]
button | "on_release" | :heavy_minus_sign: | :heavy_check_mark: | $OP
group | "child" | :heavy_minus_sign: | :heavy_check_mark: | list of {name: list of widget initialization attributes}
group | "child" | :heavy_minus_sign: | :heavy_check_mark: | list of {name: $WIDGET}
menu_bar | "child" | :heavy_minus_sign: | :heavy_check_mark: | list of {name: list of attributes}
text | "text" | :heavy_check_mark: | :heavy_check_mark: | 
text | "append" | :heavy_minus_sign: | :heavy_check_mark: | 
text | "wrap" | :heavy_minus_sign: | :heavy_check_mark: | "NONE", "COLUMN", "PIXEL", "BOUNDS"
text | "key" | :heavy_minus_sign: | :heavy_check_mark: | 
text | "textsize" | :heavy_minus_sign: | :heavy_check_mark: | 
text | "textfont" | :heavy_minus_sign: | :heavy_check_mark: | 
text | "textcolor" | :heavy_minus_sign: | :heavy_check_mark: | 
text | "cursor_state" | :heavy_minus_sign: | :heavy_check_mark: | 
text | "cursor_color" | :heavy_minus_sign: | :heavy_check_mark: | 
text | "cursor_style" | :heavy_minus_sign: | :heavy_check_mark: | 
text | "cursor_visible" | :heavy_minus_sign: | :heavy_check_mark: | 
text | "on_resize" | :heavy_minus_sign: | :heavy_check_mark: | $OP
text | "on_keydown" | :heavy_minus_sign: | :heavy_check_mark: | :heavy_minus_sign: | :heavy_check_mark: | $OP
text | "on_mousewheel" | :heavy_minus_sign: | :heavy_check_mark: | $OP
text | "on_drag" | :heavy_minus_sign: | :heavy_check_mark: | $OP
text | "on_push" | :heavy_minus_sign: | :heavy_check_mark: | $OP
text | "on_release" | :heavy_minus_sign: | :heavy_check_mark: | $OP
text | "mNBufferLines" | :heavy_check_mark: | :heavy_minus_sign: | 
text | "mMaxsize" | :heavy_check_mark: | :heavy_minus_sign: | 
text | "selection" | :heavy_check_mark: | :heavy_minus_sign: | 
text | "start" | :heavy_check_mark: | :heavy_minus_sign: | 
text | "end" | :heavy_check_mark: | :heavy_minus_sign: | 
text | "row" | :heavy_check_mark: | :heavy_minus_sign: | 
text | "col" | :heavy_check_mark: | :heavy_minus_sign: | 
pack | "spacing" | :heavy_minus_sign: | :heavy_check_mark: | 
table_row | "rows" | :heavy_minus_sign: | :heavy_check_mark: | 
table_row | "cols" | :heavy_minus_sign: | :heavy_check_mark: | 
table_row | "shape" | :heavy_check_mark: | :heavy_check_mark: | 
table_row | "vector" | :heavy_minus_sign: | :heavy_check_mark: | 
table_row | "on_draw_cell" | :heavy_minus_sign: | :heavy_check_mark: | $OP

## on_post handlers
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

## other attributes

name | type | desc
------------ | ------------- | -------------
"get" | attr_array
"exec" | $OP | w.child("tx").set({exec:op(){$this.get(["text"]);}});

