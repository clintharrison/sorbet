# typed: true
extend T::Sig

sig {returns(String)}
def bar
  "hello world"
end

sig {returns(String)}
def foo
  bar
end
