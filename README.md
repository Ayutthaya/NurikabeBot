NurikabeBot
~~~~~~~~~~~

NurikabeBot is a solver for the Nurikabe game.

Nurikabe is a pencil game: https://en.wikipedia.org/wiki/Nurikabe_(puzzle)

It is an NP-complete problem: http://cage.ugent.be/~klein/papers/nurikabe.pdf

How this solver works
~~~~~~~~~~~~~~~~~~~~~

It belongs to the category of Expert Sytems which emulate the decisions of a human.

It is based on a set of rules to make deductions about the color of a cell.

For instance, if a cell would form a 2x2 black block, it must necessarily be painted white.

When no more deduction can be made thanks to these rules, it makes an hypothesis about the color of a cell.

The choice of the hypothesis is based on the most constraining choice.

If a hypothesis leads to an impossible situation, the hypothesis is inverted.

It is an example of backtracking algorithm.
