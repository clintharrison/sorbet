# typed: true
extend T::Sig

class Fun
  attr_accessor :home
end

sig {returns(String)}
def bar
  fun = Fun.new
  fun.home = false
  x = 1
  y = x
  z = y + x
  "hello world"
end

sig {returns(String)}
def foo
  bar
end
