# typed: true
# frozen_string_literal: true

class Dog
  def self.bark(a, b)
    [a, b]
  end
end

Dog.bark(1, 2)

# module Zoo
#   include Kernel

#   def run
#     x = Dog.bark(1, 2)
#     p x
#   end
# end