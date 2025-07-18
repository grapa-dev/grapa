from __future__ import annotations

import sys

import pytest

import env  # noqa: F401
from pybind11_tests import ConstructorStats
from pybind11_tests import methods_and_attributes as m

NO_GETTER_MSG = (
    "unreadable attribute" if sys.version_info < (3, 11) else "object has no getter"
)
NO_SETTER_MSG = (
    "can't set attribute" if sys.version_info < (3, 11) else "object has no setter"
)
NO_DELETER_MSG = (
    "can't delete attribute" if sys.version_info < (3, 11) else "object has no deleter"
)


def test_methods_and_attributes():
    instance1 = m.ExampleMandA()
    instance2 = m.ExampleMandA(32)

    instance1.add1(instance2)
    instance1.add2(instance2)
    instance1.add3(instance2)
    instance1.add4(instance2)
    instance1.add5(instance2)
    instance1.add6(32)
    instance1.add7(32)
    instance1.add8(32)
    instance1.add9(32)
    instance1.add10(32)

    assert str(instance1) == "ExampleMandA[value=320]"
    assert str(instance2) == "ExampleMandA[value=32]"
    assert str(instance1.self1()) == "ExampleMandA[value=320]"
    assert str(instance1.self2()) == "ExampleMandA[value=320]"
    assert str(instance1.self3()) == "ExampleMandA[value=320]"
    assert str(instance1.self4()) == "ExampleMandA[value=320]"
    assert str(instance1.self5()) == "ExampleMandA[value=320]"

    assert instance1.internal1() == 320
    assert instance1.internal2() == 320
    assert instance1.internal3() == 320
    assert instance1.internal4() == 320
    assert instance1.internal5() == 320

    assert instance1.overloaded() == "()"
    assert instance1.overloaded(0) == "(int)"
    assert instance1.overloaded(1, 1.0) == "(int, float)"
    assert instance1.overloaded(2.0, 2) == "(float, int)"
    assert instance1.overloaded(3, 3) == "(int, int)"
    assert instance1.overloaded(4.0, 4.0) == "(float, float)"
    assert instance1.overloaded_const(-3) == "(int) const"
    assert instance1.overloaded_const(5, 5.0) == "(int, float) const"
    assert instance1.overloaded_const(6.0, 6) == "(float, int) const"
    assert instance1.overloaded_const(7, 7) == "(int, int) const"
    assert instance1.overloaded_const(8.0, 8.0) == "(float, float) const"
    assert instance1.overloaded_float(1, 1) == "(float, float)"
    assert instance1.overloaded_float(1, 1.0) == "(float, float)"
    assert instance1.overloaded_float(1.0, 1) == "(float, float)"
    assert instance1.overloaded_float(1.0, 1.0) == "(float, float)"

    assert instance1.value == 320
    instance1.value = 100
    assert str(instance1) == "ExampleMandA[value=100]"

    cstats = ConstructorStats.get(m.ExampleMandA)
    assert cstats.alive() == 2
    del instance1, instance2
    assert cstats.alive() == 0
    assert cstats.values() == ["32"]
    assert cstats.default_constructions == 1
    assert cstats.copy_constructions == 2
    assert cstats.move_constructions >= 2
    assert cstats.copy_assignments == 0
    assert cstats.move_assignments == 0


def test_copy_method():
    """Issue #443: calling copied methods fails in Python 3"""

    m.ExampleMandA.add2c = m.ExampleMandA.add2
    m.ExampleMandA.add2d = m.ExampleMandA.add2b
    a = m.ExampleMandA(123)
    assert a.value == 123
    a.add2(m.ExampleMandA(-100))
    assert a.value == 23
    a.add2b(m.ExampleMandA(20))
    assert a.value == 43
    a.add2c(m.ExampleMandA(6))
    assert a.value == 49
    a.add2d(m.ExampleMandA(-7))
    assert a.value == 42


def test_properties():
    instance = m.TestProperties()

    assert instance.def_readonly == 1
    with pytest.raises(AttributeError):
        instance.def_readonly = 2

    instance.def_readwrite = 2
    assert instance.def_readwrite == 2

    assert instance.def_property_readonly == 2
    with pytest.raises(AttributeError):
        instance.def_property_readonly = 3

    instance.def_property = 3
    assert instance.def_property == 3

    with pytest.raises(AttributeError) as excinfo:
        dummy = instance.def_property_writeonly  # unused var
    assert NO_GETTER_MSG in str(excinfo.value)

    instance.def_property_writeonly = 4
    assert instance.def_property_readonly == 4

    with pytest.raises(AttributeError) as excinfo:
        dummy = instance.def_property_impossible  # noqa: F841 unused var
    assert NO_GETTER_MSG in str(excinfo.value)

    with pytest.raises(AttributeError) as excinfo:
        instance.def_property_impossible = 5
    assert NO_SETTER_MSG in str(excinfo.value)


def test_static_properties():
    assert m.TestProperties.def_readonly_static == 1
    with pytest.raises(AttributeError) as excinfo:
        m.TestProperties.def_readonly_static = 2
    assert NO_SETTER_MSG in str(excinfo.value)

    m.TestProperties.def_readwrite_static = 2
    assert m.TestProperties.def_readwrite_static == 2

    with pytest.raises(AttributeError) as excinfo:
        dummy = m.TestProperties.def_writeonly_static  # unused var
    assert NO_GETTER_MSG in str(excinfo.value)

    m.TestProperties.def_writeonly_static = 3
    assert m.TestProperties.def_readonly_static == 3

    assert m.TestProperties.def_property_readonly_static == 3
    with pytest.raises(AttributeError) as excinfo:
        m.TestProperties.def_property_readonly_static = 99
    assert NO_SETTER_MSG in str(excinfo.value)

    m.TestProperties.def_property_static = 4
    assert m.TestProperties.def_property_static == 4

    with pytest.raises(AttributeError) as excinfo:
        dummy = m.TestProperties.def_property_writeonly_static
    assert NO_GETTER_MSG in str(excinfo.value)

    m.TestProperties.def_property_writeonly_static = 5
    assert m.TestProperties.def_property_static == 5

    # Static property read and write via instance
    instance = m.TestProperties()

    m.TestProperties.def_readwrite_static = 0
    assert m.TestProperties.def_readwrite_static == 0
    assert instance.def_readwrite_static == 0

    instance.def_readwrite_static = 2
    assert m.TestProperties.def_readwrite_static == 2
    assert instance.def_readwrite_static == 2

    with pytest.raises(AttributeError) as excinfo:
        dummy = instance.def_property_writeonly_static  # noqa: F841 unused var
    assert NO_GETTER_MSG in str(excinfo.value)

    instance.def_property_writeonly_static = 4
    assert instance.def_property_static == 4

    # It should be possible to override properties in derived classes
    assert m.TestPropertiesOverride().def_readonly == 99
    assert m.TestPropertiesOverride.def_readonly_static == 99

    # Only static attributes can be deleted
    del m.TestPropertiesOverride.def_readonly_static
    assert hasattr(m.TestPropertiesOverride, "def_readonly_static")
    assert (
        m.TestPropertiesOverride.def_readonly_static
        is m.TestProperties.def_readonly_static
    )
    assert "def_readonly_static" not in m.TestPropertiesOverride.__dict__
    properties_override = m.TestPropertiesOverride()
    with pytest.raises(AttributeError) as excinfo:
        del properties_override.def_readonly
    assert NO_DELETER_MSG in str(excinfo.value)


def test_static_cls():
    """Static property getter and setters expect the type object as the their only argument"""

    instance = m.TestProperties()
    assert m.TestProperties.static_cls is m.TestProperties
    assert instance.static_cls is m.TestProperties

    def check_self(self):
        assert self is m.TestProperties

    m.TestProperties.static_cls = check_self
    instance.static_cls = check_self


def test_metaclass_override():
    """Overriding pybind11's default metaclass changes the behavior of `static_property`"""

    assert type(m.ExampleMandA).__name__ == "pybind11_type"
    assert type(m.MetaclassOverride).__name__ == "type"

    assert m.MetaclassOverride.readonly == 1
    assert (
        type(m.MetaclassOverride.__dict__["readonly"]).__name__
        == "pybind11_static_property"
    )

    # Regular `type` replaces the property instead of calling `__set__()`
    m.MetaclassOverride.readonly = 2
    assert m.MetaclassOverride.readonly == 2
    assert isinstance(m.MetaclassOverride.__dict__["readonly"], int)


def test_no_mixed_overloads():
    from pybind11_tests import detailed_error_messages_enabled

    with pytest.raises(RuntimeError) as excinfo:
        m.ExampleMandA.add_mixed_overloads1()
    assert (
        str(excinfo.value)
        == "overloading a method with both static and instance methods is not supported; "
        + (
            "#define PYBIND11_DETAILED_ERROR_MESSAGES or compile in debug mode for more details"
            if not detailed_error_messages_enabled
            else "error while attempting to bind static method ExampleMandA.overload_mixed1"
            "(arg0: float) -> str"
        )
    )

    with pytest.raises(RuntimeError) as excinfo:
        m.ExampleMandA.add_mixed_overloads2()
    assert (
        str(excinfo.value)
        == "overloading a method with both static and instance methods is not supported; "
        + (
            "#define PYBIND11_DETAILED_ERROR_MESSAGES or compile in debug mode for more details"
            if not detailed_error_messages_enabled
            else "error while attempting to bind instance method ExampleMandA.overload_mixed2"
            "(self: pybind11_tests.methods_and_attributes.ExampleMandA, arg0: int, arg1: int)"
            " -> str"
        )
    )


@pytest.mark.parametrize("access", ["ro", "rw", "static_ro", "static_rw"])
def test_property_return_value_policies(access):
    obj = m.TestPropRVP() if not access.startswith("static") else m.TestPropRVP

    ref = getattr(obj, access + "_ref")
    assert ref.value == 1
    ref.value = 2
    assert getattr(obj, access + "_ref").value == 2
    ref.value = 1  # restore original value for static properties

    copy = getattr(obj, access + "_copy")
    assert copy.value == 1
    copy.value = 2
    assert getattr(obj, access + "_copy").value == 1

    copy = getattr(obj, access + "_func")
    assert copy.value == 1
    copy.value = 2
    assert getattr(obj, access + "_func").value == 1


def test_property_rvalue_policy():
    """When returning an rvalue, the return value policy is automatically changed from
    `reference(_internal)` to `move`. The following would not work otherwise."""

    instance = m.TestPropRVP()
    o = instance.rvalue
    assert o.value == 1

    os = m.TestPropRVP.static_rvalue
    assert os.value == 1


# https://foss.heptapod.net/pypy/pypy/-/issues/2447
@pytest.mark.xfail("env.PYPY")
def test_dynamic_attributes():
    instance = m.DynamicClass()
    assert not hasattr(instance, "foo")
    assert "foo" not in dir(instance)

    # Dynamically add attribute
    instance.foo = 42
    assert hasattr(instance, "foo")
    assert instance.foo == 42
    assert "foo" in dir(instance)

    # __dict__ should be accessible and replaceable
    assert "foo" in instance.__dict__
    instance.__dict__ = {"bar": True}
    assert not hasattr(instance, "foo")
    assert hasattr(instance, "bar")

    with pytest.raises(TypeError) as excinfo:
        instance.__dict__ = []
    assert str(excinfo.value) == "__dict__ must be set to a dictionary, not a 'list'"

    cstats = ConstructorStats.get(m.DynamicClass)
    assert cstats.alive() == 1
    del instance
    assert cstats.alive() == 0

    # Derived classes should work as well
    class PythonDerivedDynamicClass(m.DynamicClass):
        pass

    for cls in m.CppDerivedDynamicClass, PythonDerivedDynamicClass:
        derived = cls()
        derived.foobar = 100
        assert derived.foobar == 100

        assert cstats.alive() == 1
        del derived
        assert cstats.alive() == 0


# https://foss.heptapod.net/pypy/pypy/-/issues/2447
@pytest.mark.xfail("env.PYPY")
def test_cyclic_gc():
    # One object references itself
    instance = m.DynamicClass()
    instance.circular_reference = instance

    cstats = ConstructorStats.get(m.DynamicClass)
    assert cstats.alive() == 1
    del instance
    assert cstats.alive() == 0

    # Two object reference each other
    i1 = m.DynamicClass()
    i2 = m.DynamicClass()
    i1.cycle = i2
    i2.cycle = i1

    assert cstats.alive() == 2
    del i1, i2
    assert cstats.alive() == 0


def test_bad_arg_default(msg):
    from pybind11_tests import detailed_error_messages_enabled

    with pytest.raises(RuntimeError) as excinfo:
        m.bad_arg_def_named()
    assert msg(excinfo.value) == (
        "arg(): could not convert default argument 'a: UnregisteredType' in function "
        "'should_fail' into a Python object (type not registered yet?)"
        if detailed_error_messages_enabled
        else "arg(): could not convert default argument into a Python object (type not registered "
        "yet?). #define PYBIND11_DETAILED_ERROR_MESSAGES or compile in debug mode for more information."
    )

    with pytest.raises(RuntimeError) as excinfo:
        m.bad_arg_def_unnamed()
    assert msg(excinfo.value) == (
        "arg(): could not convert default argument 'UnregisteredType' in function "
        "'should_fail' into a Python object (type not registered yet?)"
        if detailed_error_messages_enabled
        else "arg(): could not convert default argument into a Python object (type not registered "
        "yet?). #define PYBIND11_DETAILED_ERROR_MESSAGES or compile in debug mode for more information."
    )


def test_accepts_none(msg):
    a = m.NoneTester()
    assert m.no_none1(a) == 42
    assert m.no_none2(a) == 42
    assert m.no_none3(a) == 42
    assert m.no_none4(a) == 42
    assert m.no_none5(a) == 42
    assert m.ok_none1(a) == 42
    assert m.ok_none2(a) == 42
    assert m.ok_none3(a) == 42
    assert m.ok_none4(a) == 42
    assert m.ok_none5(a) == 42

    with pytest.raises(TypeError) as excinfo:
        m.no_none1(None)
    assert "incompatible function arguments" in str(excinfo.value)
    with pytest.raises(TypeError) as excinfo:
        m.no_none2(None)
    assert "incompatible function arguments" in str(excinfo.value)
    with pytest.raises(TypeError) as excinfo:
        m.no_none3(None)
    assert "incompatible function arguments" in str(excinfo.value)
    with pytest.raises(TypeError) as excinfo:
        m.no_none4(None)
    assert "incompatible function arguments" in str(excinfo.value)
    with pytest.raises(TypeError) as excinfo:
        m.no_none5(None)
    assert "incompatible function arguments" in str(excinfo.value)

    # The first one still raises because you can't pass None as a lvalue reference arg:
    with pytest.raises(TypeError) as excinfo:
        assert m.ok_none1(None) == -1
    assert (
        msg(excinfo.value)
        == """
        ok_none1(): incompatible function arguments. The following argument types are supported:
            1. (arg0: m.methods_and_attributes.NoneTester) -> int

        Invoked with: None
    """
    )

    # The rest take the argument as pointer or holder, and accept None:
    assert m.ok_none2(None) == -1
    assert m.ok_none3(None) == -1
    assert m.ok_none4(None) == -1
    assert m.ok_none5(None) == -1

    with pytest.raises(TypeError) as excinfo:
        m.no_none_kwarg(None)
    assert "incompatible function arguments" in str(excinfo.value)
    with pytest.raises(TypeError) as excinfo:
        m.no_none_kwarg(a=None)
    assert "incompatible function arguments" in str(excinfo.value)
    with pytest.raises(TypeError) as excinfo:
        m.no_none_kwarg_kw_only(None)
    assert "incompatible function arguments" in str(excinfo.value)
    with pytest.raises(TypeError) as excinfo:
        m.no_none_kwarg_kw_only(a=None)
    assert "incompatible function arguments" in str(excinfo.value)


def test_casts_none():
    """#2778: implicit casting from None to object (not pointer)"""
    a = m.NoneCastTester()
    assert m.ok_obj_or_none(a) == -1
    a = m.NoneCastTester(4)
    assert m.ok_obj_or_none(a) == 4
    a = m.NoneCastTester(None)
    assert m.ok_obj_or_none(a) == -1
    assert m.ok_obj_or_none(None) == -1


def test_str_issue(msg):
    """#283: __str__ called on uninitialized instance when constructor arguments invalid"""

    assert str(m.StrIssue(3)) == "StrIssue[3]"

    with pytest.raises(TypeError) as excinfo:
        str(m.StrIssue("no", "such", "constructor"))
    assert (
        msg(excinfo.value)
        == """
        __init__(): incompatible constructor arguments. The following argument types are supported:
            1. m.methods_and_attributes.StrIssue(arg0: int)
            2. m.methods_and_attributes.StrIssue()

        Invoked with: 'no', 'such', 'constructor'
    """
    )


def test_unregistered_base_implementations():
    a = m.RegisteredDerived()
    a.do_nothing()
    assert a.rw_value == 42
    assert a.ro_value == 1.25
    a.rw_value += 5
    assert a.sum() == 48.25
    a.increase_value()
    assert a.rw_value == 48
    assert a.ro_value == 1.5
    assert a.sum() == 49.5
    assert a.rw_value_prop == 48
    a.rw_value_prop += 1
    assert a.rw_value_prop == 49
    a.increase_value()
    assert a.ro_value_prop == 1.75


def test_ref_qualified():
    """Tests that explicit lvalue ref-qualified methods can be called just like their
    non ref-qualified counterparts."""

    r = m.RefQualified()
    assert r.value == 0
    r.refQualified(17)
    assert r.value == 17
    assert r.constRefQualified(23) == 40


def test_overload_ordering():
    "Check to see if the normal overload order (first defined) and prepend overload order works"
    assert m.overload_order("string") == 1
    assert m.overload_order(0) == 4

    assert "1. overload_order(arg0: int) -> int" in m.overload_order.__doc__
    assert "2. overload_order(arg0: str) -> int" in m.overload_order.__doc__
    assert "3. overload_order(arg0: str) -> int" in m.overload_order.__doc__
    assert "4. overload_order(arg0: int) -> int" in m.overload_order.__doc__

    with pytest.raises(TypeError) as err:
        m.overload_order(1.1)

    assert "1. (arg0: int) -> int" in str(err.value)
    assert "2. (arg0: str) -> int" in str(err.value)
    assert "3. (arg0: str) -> int" in str(err.value)
    assert "4. (arg0: int) -> int" in str(err.value)


def test_rvalue_ref_param():
    r = m.RValueRefParam()
    assert r.func1("123") == 3
    assert r.func2("1234") == 4
    assert r.func3("12345") == 5
    assert r.func4("123456") == 6


def test_is_setter():
    fld = m.exercise_is_setter.Field()
    assert fld.int_value == -99
    setter_return = fld.int_value = 100
    assert isinstance(setter_return, int)
    assert setter_return == 100
    assert fld.int_value == 100
