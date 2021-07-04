# Kythe indexer development notes

## Open questions

0. How should we handle builtins? Right now we make references to Object (etc.)
but they don't have (e.g.) a "post transform", obviously?

1. Does the order of treewalker guarantee when we visit a module or class,
we know everything in it?

2. What about methods? Is it going to require an expensive query to reference
all overrides of a method, if we try to write that? I'd prefer a solution where
we write that connection on the _override_, but Ruby is weird, and I don't know
what all of our constraints are.

3. Should we move to protobuf Entry output instead of JSON? Probably.

4. How to wire up kythe verifier in Sorbet CI...?

5. https://kythe.io/docs/schema -- The kythe schema has a lot of things I don't
really understand. Which correspond well to Ruby concepts, with inheritance and
singleton classes and mixins etc. :curlyhumidoverwhelmed:

6. How do ancestors work in Ruby? Anything weird there?

## Scratch notes

```bash
bazel build //main:sorbet && bazel-bin/main/sorbet --print=kythe-json --silence-dev-message test/testdata/basic.rb


```