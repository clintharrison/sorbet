# typed: true
# frozen_string_literal: true

#- @"class E" defines EClass
#- EClass?.node/kind record
#- EClass.subkind class
class E; end

#- @"module B" defines BModule
#- BModule?.node/kind record
#- BModule.subkind module
module B; end

#- @"def bark" defines/binding Bark
#- Bark?.node/kind function
def bark; end
