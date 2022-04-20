# Base Types

$OP and $CODE provide the basis of the grapa core language. All other syntax compiles down to a combination of these 2 types - which support an execution tree linking library calls.

The following creates an $OP that takes a parameter x and adds the numbers from 1 to x, and returns the result. 

```
> op(x){i=1;n=0;while(i<=x){n+=i;i+=1;};n;}

@<
  [op,
    @[
      @<assign,{i,1}>,
      @<assign,{n,0}>,
      @<while,
      {
        @<lteq,
        {
          @<var,{i}>,
          @<var,{x}>
        }>,
        @<scope,
        {
          @[
            @<assignappend,{n,@<var,{i}>}>,
            @<assignappend,{i,1}>
          ]
        }>
      }>,
      @<var,{n}>
    ]
  ],
  {x}
>
```

## $OP
This is the core type at the base of grapa. It does not have a class associated. Consider it as both a high level version of byte code (but represented as a list) and simular to a lambda in other languages. It's a variable that includes executable instructions. 

Understanding the inner workings of $OP is not necessary to use grapa - and this section can be skipped. This though is core to how grapa works, and an understanding is needed in order to extend the syntax of the language to support your own domain specific language extensions.

Examples:
Commands | Results | Description
------------ | ------------- | -------------
(op(){4*2})(); | 8 | Creates $OP and executes.
(@<mul,{4,2}>)(); | 8 | Same but using $OP directly.

Adding parameters to $OP.

Example: The op command is used for passing parameters into a function. When the op command is used, the name is replaced with an array where the first value is "op" and the second value is the function. This array is than followed by the parameter list. The parameters are used to initialize the namespace for the function.

Commands | Results
------------ | -------------
op(){1} | @<[op,1],{}>
op(a,b){a*b}; | @<[op,@<mul,{@<var,{a}>,@<var,{b}>}>],{a,b}>
op(){5%2}; | @<[op,@<mod,{5,2}>],{}>
op(){4*2}; | @<[op,8],{}>

In the last example above, the compiler implemented an optimization where it recognized that multiplying two constants could be completed at compile time.

See the section on syntax for additional examples.

## $CODE
A sequence of $OP items to be processed in sequence.  Normally you would use $CODE directly. It is typically embedded in an $OP type. The planner typically wraps the result in an $OP along with parameter handling for the function - but this could change at some point where it could be either $OP or $CODE. 

Example: Assigns 2 to a, than evaluates a*3, the result is 6
Commands | Results
------------ | -------------
f=@[1,2];</br>f(); | 2
f=op(){a=2;a*3;};</br>f; | @<[op,@[@<assign,{a,2}>,@<mul,{@<var,{a}>,3}>]],{}>
f(); | 6

# Base Libraries

name | C++ class
--------- | --------- 
"rule" | GrapaLibraryRuleRuleEvent
"ruleval" | GrapaLibraryRuleRuleValEvent
"ruleref" | GrapaLibraryRuleRuleRefEvent
"token" | GrapaLibraryRuleTokenEvent
"grammar" | GrapaLibraryRuleGrammarEvent
"reset" | GrapaLibraryRuleResetEvent
"shell" | GrapaLibraryRuleShellEvent
"exec" | GrapaLibraryRuleExecEvent
"post" | GrapaLibraryRulePostEvent
"sleep" | GrapaLibraryRuleSleepEvent
"exit" | GrapaLibraryRuleExitEvent
"getenv" | GrapaLibraryRuleGetEnvEvent
"putenv" | GrapaLibraryRulePutEnvEvent
"include" | GrapaLibraryRuleIncludeEvent
"name" | GrapaLibraryRuleNameEvent
"getname" | GrapaLibraryRuleGetNameEvent
"lit" | GrapaLibraryRuleLitEvent
"ref" | GrapaLibraryRuleRefEvent
"setnull" | GrapaLibraryRuleNullEvent
"var" | GrapaLibraryRuleVarEvent
"assign" | GrapaLibraryRuleAssignEvent
"assignappend" | GrapaLibraryRuleAssignAppendEvent
"assignextend" | GrapaLibraryRuleAssignExtendEvent
"createarray" | GrapaLibraryRuleCreateArrayEvent
"createlist" | GrapaLibraryRuleCreateListEvent
"createxml" | GrapaLibraryRuleCreateXmlEvent
"createel" | GrapaLibraryRuleCreateElEvent
"createtag" | GrapaLibraryRuleCreateTagEvent
"createop" | GrapaLibraryRuleCreateOpEvent
"createcode" | GrapaLibraryRuleCreateCodeEvent
"createrule" | GrapaLibraryRuleCreateRuleEvent
"extend" | GrapaLibraryRuleExtendEvent
"remove" | GrapaLibraryRuleRemoveEvent
"prepend" | GrapaLibraryRulePrependEvent
"pop" | GrapaLibraryRulePopEvent
"call" | GrapaLibraryRuleCallEvent
"search" | GrapaLibraryRuleSearchEvent
"findall" | GrapaLibraryRuleFindAllEvent
"sort" | GrapaLibraryRuleSortEvent
"argsort" | GrapaLibraryRuleArgSortEvent
"unique" | GrapaLibraryRuleUniqueEvent
"group" | GrapaLibraryRuleGroupEvent
"class" | GrapaLibraryRuleClassEvent
"obj" | GrapaLibraryRuleObjEvent
"global" | GrapaLibraryRuleGlobalEvent
"parent" | GrapaLibraryRuleParentEvent
"this" | GrapaLibraryRuleThisEvent
"local" | GrapaLibraryRuleLocalEvent
"static" | GrapaLibraryRuleStaticEvent
"const" | GrapaLibraryRuleConstEvent
"setconst" | GrapaLibraryRuleSetConstEvent
"return" | GrapaLibraryRuleReturnEvent
"break" | GrapaLibraryRuleBreakEvent
"if" | GrapaLibraryRuleIfEvent
"while" | GrapaLibraryRuleWhileEvent
"scope" | GrapaLibraryRuleScopeEvent
"switch" | class GrapaLibraryRuleSwitchEvent
"case" | GrapaLibraryRuleCaseEvent
"plan" | GrapaLibraryRulePlanEvent
"wrap" | GrapaLibraryRuleWrapEvent
"op" | GrapaLibraryRuleOpEvent
"map" | GrapaLibraryRuleMapEvent
"reduce" | GrapaLibraryRuleReduceEvent
"filter" | GrapaLibraryRuleFilterEvent
"range" | GrapaLibraryRuleRangeEvent
"isint" | GrapaLibraryRuleIsIntEvent
"iferr" | GrapaLibraryRuleIfErrEvent
"message" | GrapaLibraryRuleMessageEvent
"string" | GrapaLibraryRuleStringEvent
"echo" | GrapaLibraryRuleEchoEvent
"console" | GrapaLibraryRuleConsoleEvent
"prompt" | GrapaLibraryRulePromptEvent
"genprime" | GrapaLibraryRuleGenPrimeEvent
"staticprime" | GrapaLibraryRuleStaticPrimeEvent
"isprime" | GrapaLibraryRuleIsPrimeEvent
"isaks" | GrapaLibraryRuleIsAksEvent
"random" | GrapaLibraryRuleRandomEvent
"setbit" | GrapaLibraryRuleSetBitEvent
"clearbit" | GrapaLibraryRuleClearBitEvent
"genbits" | GrapaLibraryRuleGenBitsEvent
"genkeys" | GrapaLibraryRuleGenKeysEvent
"encode" | GrapaLibraryRuleEncodeEvent
"encoderaw" | GrapaLibraryRuleEncodeEvent
"decode" | GrapaLibraryRuleDecodeEvent
"sign" | GrapaLibraryRuleSignEvent
"signadd" | GrapaLibraryRuleSignAddEvent
"verify" | GrapaLibraryRuleVerifyEvent
"verifyrecover" | GrapaLibraryRuleVerifyRecoverEvent
"secret" | GrapaLibraryRuleSecretEvent
"setfloat" | GrapaLibraryRuleSetFloatEvent
"setfix" | GrapaLibraryRuleSetFixEvent
"root" | GrapaLibraryRuleRootEvent
"pow" | GrapaLibraryRulePowEvent
"mod" | GrapaLibraryRuleModEvent
"modpow" | GrapaLibraryRuleModPowEvent
"modinv" | GrapaLibraryRuleModInvEvent
"abs" | GrapaLibraryRuleAbsEvent
"gcd" | GrapaLibraryRuleGcdEvent
"e" | GrapaLibraryRuleEEvent
"pi" | GrapaLibraryRulePiEvent
"ln" | GrapaLibraryRuleLnEvent
"log" | GrapaLibraryRuleLogEvent
"add" | GrapaLibraryRuleAddEvent
"sub" | GrapaLibraryRuleSubEvent
"mul" | GrapaLibraryRuleMulEvent
"div" | GrapaLibraryRuleDivEvent
"sin" | GrapaLibraryRuleSinEvent
"cos" | GrapaLibraryRuleCosEvent
"tan" | GrapaLibraryRuleTanEvent
"cot" | GrapaLibraryRuleCotEvent
"sec" | GrapaLibraryRuleSecEvent
"csc" | GrapaLibraryRuleCscEvent
"asin" | GrapaLibraryRuleASinEvent
"acos" | GrapaLibraryRuleACosEvent
"atan" | GrapaLibraryRuleATanEvent
"acot" | GrapaLibraryRuleACotEvent
"asec" | GrapaLibraryRuleASecEvent
"acsc" | GrapaLibraryRuleACscEvent
"sinh" | GrapaLibraryRuleSinHEvent
"cosh" | GrapaLibraryRuleCosHEvent
"tanh" | GrapaLibraryRuleTanHEvent
"coth" | GrapaLibraryRuleCotHEvent
"sech" | GrapaLibraryRuleSecHEvent
"csch" | GrapaLibraryRuleCscHEvent
"asinh" | GrapaLibraryRuleASinHEvent
"acosh" | GrapaLibraryRuleACosHEvent
"atanh" | GrapaLibraryRuleATanHEvent
"acoth" | GrapaLibraryRuleACotHEvent
"asech" | GrapaLibraryRuleASecHEvent
"acsch" | GrapaLibraryRuleACscHEvent
"atan2" | GrapaLibraryRuleATan2Event
"hypot" | GrapaLibraryRuleHypotEvent
"bsl" | GrapaLibraryRuleBslEvent
"bsr" | GrapaLibraryRuleBsrEvent
"bor" | GrapaLibraryRuleBorEvent
"band" | GrapaLibraryRuleBandEvent
"or" | GrapaLibraryRuleOrEvent
"and" | GrapaLibraryRuleAndEvent
"xor" | GrapaLibraryRuleXOrEvent
"inv" | GrapaLibraryRuleInvEvent
"t" | GrapaLibraryRuleTransposeEvent
"rref" | GrapaLibraryRuleRrefEvent
"det" | GrapaLibraryRuleDetEvent
"rank" | GrapaLibraryRuleRankEvent
"solve" | GrapaLibraryRuleSolveEvent
"cov" | GrapaLibraryRuleCovEvent
"neg" | GrapaLibraryRuleNegEvent
"bits" | GrapaLibraryRuleBitsEvent
"bytes" | GrapaLibraryRuleBytesEvent
"len" | GrapaLibraryRuleLenEvent
"bool" | GrapaLibraryRuleBoolEvent
"not" | GrapaLibraryRuleNotEvent
"int" | GrapaLibraryRuleIntEvent
"raw" | GrapaLibraryRuleRawEvent
"base" | GrapaLibraryRuleBaseEvent
"str" | GrapaLibraryRuleStrEvent
"list" | GrapaLibraryRuleListEvent
"array" | GrapaLibraryRuleArrayEvent
"vector" | GrapaLibraryRuleVectorEvent
"xml" | GrapaLibraryRuleXmlEvent
"float" | GrapaLibraryRuleFloatEvent
"fix" | GrapaLibraryRuleFloatEvent
"time" | GrapaLibraryRuleTimeEvent
"type" | GrapaLibraryRuleTypeEvent
"describe" | GrapaLibraryRuleDescribeEvent
"left" | GrapaLibraryRuleLeftEvent
"right" | GrapaLibraryRuleRightEvent
"mid" | GrapaLibraryRuleMidEvent
"midtrim" | GrapaLibraryRuleMidTrimEvent
"rtrim" | GrapaLibraryRuleRTrimEvent
"ltrim" | GrapaLibraryRuleLTrimEvent
"trim" | GrapaLibraryRuleTrimEvent
"rrot" | GrapaLibraryRuleRRotateEvent
"lrot" | GrapaLibraryRuleLRotateEvent
"rpad" | GrapaLibraryRuleRPadEvent
"lpad" | GrapaLibraryRuleLPadEvent
"reverse" | GrapaLibraryRuleReverseEvent
"replace" | GrapaLibraryRuleReplaceEvent
"split" | GrapaLibraryRuleSplitEvent
"join" | GrapaLibraryRuleJoinEvent
"shape" | GrapaLibraryRuleShapeEvent
"reshape" | GrapaLibraryRuleReShapeEvent
"dot" | GrapaLibraryRuleDotEvent
"identity" | GrapaLibraryRuleIdentityEvent
"diagonal" | GrapaLibraryRuleDiagonalEvent
"triu" | GrapaLibraryRuleTriUEvent
"tril" | GrapaLibraryRuleTriLEvent
"eigh" | GrapaLibraryRuleEigHEvent
"sum" | GrapaLibraryRuleSumEvent
"mean" | GrapaLibraryRuleMeanEvent
"lower" | GrapaLibraryRuleLowerEvent
"upper" | GrapaLibraryRuleUpperEvent
"eq" | GrapaLibraryRuleEqEvent
"neq" | GrapaLibraryRuleNEqEvent
"gteq" | GrapaLibraryRuleGtEqEvent
"gt" | GrapaLibraryRuleGtEvent
"lteq" | GrapaLibraryRuleLtEqEvent
"lt" | GrapaLibraryRuleLtEvent
"cmp" | GrapaLibraryRuleCmpEvent
"utc" | GrapaLibraryRuleUtcEvent
"tz" | GrapaLibraryRuleTzEvent
"file_table" | GrapaLibraryRuleTableEvent
"file_pwd" | GrapaLibraryRulePwdEvent
"file_cd" | GrapaLibraryRuleCdEvent
"file_phd" | GrapaLibraryRulePhdEvent
"file_chd" | GrapaLibraryRuleChdEvent
"file_ls" | GrapaLibraryRuleLsEvent
"file_mk" | GrapaLibraryRuleMkEvent
"file_rm" | GrapaLibraryRuleRmEvent
"file_mkfield" | GrapaLibraryRuleMkFieldEvent
"file_rmfield" | GrapaLibraryRuleRmFieldEvent
"file_info" | GrapaLibraryRuleInfoEvent
"file_set" | GrapaLibraryRuleSetEvent
"file_get" | GrapaLibraryRuleGetEvent
"file_split" | GrapaLibraryRuleFileSplitEvent
"file_debug" | GrapaLibraryRuleDebugEvent
"net_mac" | GrapaLibraryRuleMacEvent
"net_interfaces" | GrapaLibraryRuleInterfacesEvent
"net_connect" | GrapaLibraryRuleConnectEvent
"net_bind" | GrapaLibraryRuleBindEvent
"net_listen" | GrapaLibraryRuleListenEvent
"net_onlisten" | GrapaLibraryRuleOnListenEvent
"net_disconnect" | GrapaLibraryRuleDisconnectEvent
"net_proxy" | GrapaLibraryRuleProxyEvent
"net_certificate" | GrapaLibraryRuleCertificateEvent
"net_private" | GrapaLibraryRulePrivateEvent
"net_trusted" | GrapaLibraryRuleTrustedEvent
"net_verify" | GrapaLibraryRuleNetVerifyEvent
"net_chain" | GrapaLibraryRuleNetChainEvent
"net_host" | GrapaLibraryRuleHostEvent
"net_send" | GrapaLibraryRuleSendEvent
"net_receive" | GrapaLibraryRuleReceiveEvent
"net_pending" | GrapaLibraryRulePendingEvent
"net_onreceive" | GrapaLibraryRuleOnReceiveEvent
"http_read" | GrapaLibraryRuleHttpReadEvent
"http_send" | GrapaLibraryRuleHttpSendEvent
"http_message" | GrapaLibraryRuleHttpMessageEvent
"thread_trylock" | GrapaLibraryRuleTryLockEvent
"thread_lock" | GrapaLibraryRuleLockEvent
"thread_unlock" | GrapaLibraryRuleUnLockEvent
"thread_wait" | GrapaLibraryRuleWaitEvent
"thread_signal" | GrapaLibraryRuleSignalEvent
"thread_waiting" | GrapaLibraryRuleWaitingEvent
"thread_start" | GrapaLibraryRuleStartEvent
"thread_stop" | GrapaLibraryRuleStopEvent
"thread_started" | GrapaLibraryRuleStartedEvent
"thread_suspend" | GrapaLibraryRuleSuspendEvent
"thread_resume" | GrapaLibraryRuleResumeEvent
"thread_suspended" | GrapaLibraryRuleSuspendedEvent
"widget_new" | GrapaLibraryRuleWidgetNewEvent
"widget_show" | GrapaLibraryRuleWidgetShowEvent
"widget_hide" | GrapaLibraryRuleWidgetHideEvent
"widget_redraw" | GrapaLibraryRuleWidgetRedrawEvent
"widget_resizable" | GrapaLibraryRuleWidgetResizableEvent
"widget_resize" | GrapaLibraryRuleWidgetResizeEvent
"widget_parent" | GrapaLibraryRuleWidgetParentEvent
"widget_child" | GrapaLibraryRuleWidgetChildEvent
"widget_next" | GrapaLibraryRuleWidgetNextEvent
"widget_focus" | GrapaLibraryRuleWidgetFocusEvent
"widget_get" | GrapaLibraryRuleWidgetGetEvent
"widget_set" | GrapaLibraryRuleWidgetSetEvent
"widget_handle" | GrapaLibraryRuleWidgetHandleEvent
"widget_callback" | GrapaLibraryRuleWidgetCallbackEvent
"widget_event_key" | GrapaLibraryRuleWidgetEventKeyEvent
"widget_append" | GrapaLibraryRuleWidgetAppendEvent
"widget_post" | GrapaLibraryRuleWidgetPostEvent
"widget_clear" | GrapaLibraryRuleWidgetClearEvent

