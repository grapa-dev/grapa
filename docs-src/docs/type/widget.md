---
tags:
  - user
  - highlevel
---
# $WIDGET

A specialized type for GUI components and interactive elements in Grapa applications.

## Overview

`$WIDGET` is a data type designed for creating and managing graphical user interface components. It provides a structured way to define UI elements with properties, behaviors, and hierarchical relationships.

## Basic Operations

### Creation
```grapa
/* Create basic widget */
button = {name:"button", type:"click", label:"Click Me"};

/* Create widget with properties */
input = {name:"input", type:"text", placeholder:"Enter text", value:""};

/* Create widget with children */
container = {name:"container", type:"div", children:[button, input]};
```

### Addition Operations (`+=`)
```grapa
widget = {name:"form", type:"form"};

/* Add widget with name and value */
widget += ("button", "Submit");           /* Add button widget */

/* Add widget at specific position */
widget += ("input", "text", widget[0]);   /* Add input before button */

/* Add widget with complex properties */
widget += ("label", {text:"Name:", for:"name"}, widget[1]);
```

### Concatenation Operations (`++=`)
```grapa
form1 = {name:"login", type:"form"};
form2 = {name:"register", type:"form"};

/* Extend widget with another widget */
form1 ++= form2;                          /* Combine form widgets */
```

## Widget Properties

### Common Properties
```grapa
widget = {
    name: "my_widget",           /* Unique identifier */
    type: "button",              /* Widget type */
    label: "Click Me",           /* Display text */
    value: "default",            /* Current value */
    enabled: true,               /* Interaction state */
    visible: true,               /* Visibility state */
    style: {                     /* Styling properties */
        color: "blue",
        size: "large",
        position: "center"
    },
    events: {                    /* Event handlers */
        click: op(){/* handler */},
        change: op(){/* handler */}
    }
};
```

### Widget Types
```grapa
/* Button widgets */
button = {name:"btn", type:"button", label:"Click"};

/* Input widgets */
text_input = {name:"input", type:"text", placeholder:"Enter text"};
number_input = {name:"num", type:"number", min:0, max:100};
checkbox = {name:"check", type:"checkbox", checked:false};

/* Container widgets */
div = {name:"container", type:"div", children:[]};
form = {name:"form", type:"form", method:"POST"};
table = {name:"table", type:"table", rows:3, cols:2};

/* Display widgets */
label = {name:"label", type:"label", text:"Display text"};
image = {name:"img", type:"image", src:"path/to/image.png"};
```

## Widget Hierarchy

### Parent-Child Relationships
```grapa
/* Create hierarchical widget structure */
form = {name:"main_form", type:"form"};

/* Add child widgets */
form += ("name_input", "text", {placeholder:"Enter name"});
form += ("email_input", "text", {placeholder:"Enter email"});
form += ("submit_btn", "button", {label:"Submit"});

/* Access child widgets */
name_field = form.children[0];
email_field = form.children[1];
submit_button = form.children[2];
```

### Widget Navigation
```grapa
widget = {name:"container", type:"div"};

/* Find widget by name */
button = widget.find("submit_button");

/* Find widget by type */
buttons = widget.find_all("button");

/* Get parent widget */
parent = widget.parent;

/* Get sibling widgets */
siblings = widget.siblings;
```

## Event Handling

### Event Definition
```grapa
widget = {
    name: "interactive_button",
    type: "button",
    label: "Click Me",
    events: {
        click: op(event){
            "Button clicked!".echo();
            event.target.value = "Clicked";
        },
        mouseover: op(event){
            event.target.style.color = "red";
        },
        mouseout: op(event){
            event.target.style.color = "black";
        }
    }
};
```

### Dynamic Event Binding
```grapa
widget = {name:"button", type:"button", label:"Click"};

/* Bind event dynamically */
widget.bind("click", op(event){
    "Dynamic click handler".echo();
});

/* Unbind event */
widget.unbind("click");
```

## Widget Styling

### Style Properties
```grapa
widget = {
    name: "styled_widget",
    type: "button",
    label: "Styled Button",
    style: {
        /* Layout */
        width: "200px",
        height: "50px",
        margin: "10px",
        padding: "5px",
        
        /* Appearance */
        background: "#007bff",
        color: "white",
        border: "1px solid #0056b3",
        borderRadius: "5px",
        
        /* Typography */
        fontSize: "16px",
        fontWeight: "bold",
        textAlign: "center",
        
        /* Position */
        position: "relative",
        top: "0px",
        left: "0px"
    }
};
```

### Dynamic Styling
```grapa
widget = {name:"dynamic", type:"button", label:"Dynamic"};

/* Change style properties */
widget.style.background = "red";
widget.style.color = "white";

/* Apply style class */
widget.add_class("highlighted");
widget.remove_class("highlighted");
widget.toggle_class("active");
```

## Widget Validation

### Validation Rules
```grapa
input = {
    name: "email_input",
    type: "text",
    placeholder: "Enter email",
    validation: {
        required: true,
        pattern: "^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$",
        message: "Please enter a valid email address",
        validate: op(value){
            return value.contains("@") && value.contains(".");
        }
    }
};
```

### Validation Methods
```grapa
widget = {name:"input", type:"text"};

/* Check if widget is valid */
is_valid = widget.validate();

/* Get validation errors */
errors = widget.get_errors();

/* Clear validation state */
widget.clear_validation();
```

## Advanced Features

### Widget Templates
```grapa
/* Create reusable widget template */
button_template = {
    type: "button",
    style: {
        background: "#007bff",
        color: "white",
        padding: "10px 20px",
        border: "none",
        borderRadius: "5px"
    }
};

/* Use template to create widgets */
primary_btn = button_template.copy();
primary_btn.name = "primary";
primary_btn.label = "Primary Action";

secondary_btn = button_template.copy();
secondary_btn.name = "secondary";
secondary_btn.label = "Secondary Action";
secondary_btn.style.background = "#6c757d";
```

### Widget Composition
```grapa
/* Create complex widget from simpler ones */
form_widget = {
    name: "user_form",
    type: "form",
    children: [
        {name:"name_label", type:"label", text:"Name:"},
        {name:"name_input", type:"text", placeholder:"Enter name"},
        {name:"email_label", type:"label", text:"Email:"},
        {name:"email_input", type:"text", placeholder:"Enter email"},
        {name:"submit_btn", type:"button", label:"Submit"}
    ]
};
```

### Widget Serialization
```grapa
widget = {name:"my_widget", type:"button", label:"Click"};

/* Convert widget to JSON */
json_data = widget.to_json();

/* Create widget from JSON */
new_widget = widget.from_json(json_data);

/* Export widget configuration */
config = widget.export_config();

/* Import widget configuration */
widget.import_config(config);
```

## Performance Considerations

- **Lazy Rendering**: Widgets are rendered only when needed
- **Event Delegation**: Efficient event handling for large widget trees
- **Memory Management**: Automatic cleanup of unused widgets
- **Batch Updates**: Multiple property changes are batched for performance

## Integration with Other Types

### Widget Arrays
```grapa
/* Create array of widgets */
buttons = [
    {name:"btn1", type:"button", label:"Button 1"},
    {name:"btn2", type:"button", label:"Button 2"},
    {name:"btn3", type:"button", label:"Button 3"}
];

/* Process widgets */
buttons.map(op(widget){
    widget.style.background = "blue";
});
```

### Widget Lists
```grapa
/* Create list of widgets */
widget_list = {
    header: {name:"header", type:"header", text:"Application"},
    sidebar: {name:"sidebar", type:"nav", items:["Home", "About", "Contact"]},
    content: {name:"content", type:"main", children:[]},
    footer: {name:"footer", type:"footer", text:"Copyright 2024"}
};
```

## See also
- [$ARRAY object](array.md)
- [$GOBJ object](gobj.md)
- [$VECTOR object](vector.md)
- [Assignment Operators](../operators/assignment.md)
- [Event Handling](../advanced_topics.md)
