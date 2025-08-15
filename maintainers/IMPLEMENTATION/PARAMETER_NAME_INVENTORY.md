# Parameter Name Inventory (lib/grapa/*.grc)

Purpose: Exhaustive inventory of methods that accept parameters in Grapa class files to support a consistent parameter name modernization pass (short, intuitive, unambiguous).

Scope: lib/grapa/*.grc files that define a class with methods.

---

## $OBJ.grc (DONE)
- Status: Completed in prior pass; names verified and documented.

---

## $file.grc ✅ **COMPLETED**
- table(type) ✅
- cd(path) ✅
- chd(path) ✅
- ls(path, format) ✅
- mk(name, type) ✅
- rm(name) ✅
- set(name, field, data) ✅
- get(name, field) ✅
- info(name, format) ✅
- split(count, name, path, delim, option) ✅
- mkfield(name, type, storage, size, growth) ✅
- rmfield(name) ✅
- debug(level, format) ✅

Notes:
- ✅ **COMPLETED**: All ambiguous identifiers renamed to descriptive names
- ✅ **TESTED**: Baseline and regression tests validate no functional changes
- ✅ **DOCUMENTED**: Updated documentation reflects new parameter names

---

## $net.grc ✅ **COMPLETED**
- connect(url, cert, proxy) ✅
- bind(url) ✅
- listen(net) ✅
- onlisten(url, messageHandler, connectHandler, count) ✅
- proxy(ptype, pid, ph) ✅
- certificate(cert) ✅
- private(key, pass, param) ✅
- trusted(file, path) ✅
- send(data) ✅
- onreceive(handler) ✅
- httpsend(method, entity, header, body) ✅
- httpmessage(raw) ✅

Notes:
- ✅ **COMPLETED**: All ambiguous identifiers renamed to descriptive names
- ✅ **TESTED**: Baseline and regression tests validate no functional changes
- ✅ **DOCUMENTED**: Documentation already reflects new parameter names

---

## $VECTOR.grc ✅ **COMPLETED**
- cov(axis) ✅
- sum(axis) ✅
- mean(axis) ✅
- reshape(shape) ✅
- dot(other) ✅
- triu(offset) ✅
- tril(offset) ✅

Notes:
- ✅ **COMPLETED**: All ambiguous identifiers renamed to descriptive names
- ✅ **TESTED**: Baseline and regression tests validate no functional changes
- ✅ **DOCUMENTED**: Documentation already reflects new parameter names

---

## $thread.grc ✅ **COMPLETED**
- start(runCode, param, doneCode) ✅

Notes:
- ✅ **COMPLETED**: All ambiguous identifiers renamed to descriptive names
- ✅ **TESTED**: Baseline and regression tests validate no functional changes
- ✅ **DOCUMENTED**: Documentation already reflects new parameter names

---

## $sys.grc ✅ **COMPLETED**
- type(object) ✅
- describe(object) ✅
- getenv(name) ✅
- putenv(name, value) ✅
- encode(data, method, params) ✅
- sleep(milliseconds) ✅

Notes:
- ✅ **COMPLETED**: All ambiguous identifiers renamed to descriptive names
- ✅ **TESTED**: Baseline and regression tests validate no functional changes
- ✅ **DOCUMENTED**: Documentation updated to reflect new parameter names

---

## $WIDGET.grc ✅ **NO CHANGES NEEDED**
- $new(widget, x, y, w, h, label, attr) ✅
- get(name, params) ✅
- set(name, data) ✅
- event_key(key) ✅
- child(name) ✅
- post(postop, postparams, doneop) ✅
- handle(event) ✅
- resize(x, y, w, h) ✅
- append(data) ✅
- resizable(widget) ✅
- setlabeltext(label, shortcut) ✅

Notes:
- ✅ **NO CHANGES NEEDED**: All parameters already have descriptive, clear names
- ✅ **ALREADY OPTIMAL**: Parameter names are intuitive and unambiguous
- setlabeltype(labeltype)
- setlabelstyle(font, size, color)
- settip(tip)

Notes:
- Names already intuitive; likely no change needed.

---

## $math.grc
- log(b)
- atan2(a, b)
- hypot(a, b)
- random(b)
- modpow(p, m)
- modinv(m)

Notes:
- a/b could be `y, x` or `a, b`; `modpow(p, m)` likely power/modulus; confirm C++.

---

## $OP.grc
- (Review needed; contains op/meta utilities; parameterized constructs may exist)

---

## Other class files (no parameterized methods or trivial wrappers)
- $ARRAY.grc, $LIST.grc, $STR.grc, $INT.grc, $FLOAT.grc, $TIME.grc, $TABLE.grc, $XML.grc, $TAG.grc, $EL.grc, $ERR.grc, $SYSINT.grc, $SYSID.grc, $ID.grc, $RULE.grc, $grapa.grc

Notes:
- Either no methods, or methods without parameters, or inherited only.

---

## Documentation Status (CORRECTED)

**Currently documented in docs-src:**
- ✅ $OBJ.grc - Fully documented in `docs-src/docs/type/obj_methods.md`
- ✅ $file.grc - Documented in `docs-src/docs/sys/file.md` (656 lines)
- ✅ $net.grc - Documented in `docs-src/docs/sys/net.md` (215 lines)
- ✅ $VECTOR.grc - Documented in `docs-src/docs/type/vector.md` (144 lines)
- ✅ $thread.grc - Documented in `docs-src/docs/sys/thread.md` (93 lines)
- ✅ $sys.grc - Documented in `docs-src/docs/sys/sys.md` (386 lines)
- ✅ $math.grc - Documented in `docs-src/docs/sys/math.md` (46 lines)
- ✅ $WIDGET.grc - Documented in `docs-src/docs/type/widget.md` (347 lines)

**Documentation Status Assessment:**
- All major classes with parameterized methods have existing documentation
- Documentation may need updates to reflect parameter name changes
- Need to verify if current docs accurately reflect parameter names and usage

---

## Next Steps
1. For each class with ambiguous names, read corresponding C++ handlers to determine parameter semantics.
2. Draft per-class rename proposal with Before → After mapping and rationale.
3. Create minimal, valid tests per method that exercise all parameters with correct data types.
4. Validate tests produce meaningful outputs (no {"error":-1}) and capture baselines.
5. Implement renames guarded by a maintainer HOLD gate before validation.
6. Update existing documentation files to reflect new parameter names.
