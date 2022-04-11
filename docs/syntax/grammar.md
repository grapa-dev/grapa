# Default Grammar Rules
To view a text version of the rules loaded into grapa on startup, run the following at the prompt:

```
> $sys().getenv("$STATICLIB")."$grapa.grc".str()
```

Or go here:
[Grapa Grammar](../../lib/grapa/$grapa.grc)

The system will first check for a match on the "start" rule. Which is a global variable...if that global variable is of type $RULE, than it will be come the first rule for scripts. This is an easy way to provide an override on command processing. If "start" rule does not provide a match, than the system will evaluage using the "$start" rule. 

The default rules may be subject to change, and so are not listed here. Use the above command to retrieve/view.  To write the result to a file, use the following:

```
> $file().set("grapa.txt",$sys().getenv("$STATICLIB")."$grapa.grc".str())
```

This will place the file in the current working directory. If you want to place in a different directly, refer to the $file class for commands to do that.

If you modify the file, you can try it out by placing it in "lib/grapa/" under the same directory that the above command would have written the file to, and then restart grapa. If grapa finds this file with this name in that location, it will use that file instead of the default. 

If you are familiar with YACC or BNF, following the grammar logic should be rather straight forward. Each rule is evaluated until a successful match, and all matches produce an execution play where the code associated with each rule match is included in the execution plan. To optimize grammar resolution, rules carefully constructed to avoid re-evaluating rules more than necessary by placing the most complex rules first and following with common rule patters. It is also important to avoid infinite recursive situations - mostly this is avoided by not referencing the same rule as the first token to evaluate. 

There are a few standard tokens that are defined - mostly the tokens provide special handling for either the lexical engine or the rules engine. One example is the space character, which when found a bit is set in the token skip the value for rules evaluation. This can be changed - but then would require including including the space token in the rules. 
