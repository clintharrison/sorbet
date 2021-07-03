# typed: true
# frozen_string_literal: true

def greet(name)
  "hello #{name}"
end

def greets
  greet 'Sorbet'
  greet <<~GREETING
    world
  GREETING
end

greets

class Zombie
  def foo(param); end

  def bar(zzz); end
end

module M; end

module Home
  extend T::Sig
  extend T::Helpers

  include M
end

module Kitchen
  class Fruit; end
  class Banana < Fruit; end
  class Berry < Fruit; end
  class Apple < Berry; end
end
