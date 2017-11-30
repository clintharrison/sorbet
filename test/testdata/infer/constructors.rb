# @typed
class Foo1
end

class Foo2
  def self.new(a)
  end
end

class Foo3
  def initialize(a, b)
  end
end

class Bar
  def foo
    Foo1.new
    Foo2.new(1 )
    Foo3.new(1, 2)
  end
end

class InstanceMethod
  def test_imethod_new
    o = Object.new
    o.new # error: Method new does not exist
  end
end
