# typed: true
extend T::Sig

sig {returns(String)}
def run
  "1.2.3"
end

sig {returns(String)}
def inferModuleVersion
  run
end
