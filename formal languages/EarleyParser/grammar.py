from typing import List, Set


class Rule:
    def __init__(self, lhs, rhs):
        self.lhs = lhs
        self.rhs = rhs

    def __repr__(self):  # debug thing
        return f"{self.lhs} -> {''.join(self.rhs) if self.rhs else 'ε'}"


class Grammar:
    def __init__(self):
        self.nonterminals: Set[str] = set()
        self.terminals: Set[str] = set()
        self.rules: List[Rule] = []
        self.start_symbol: str = ''

    def add_rule(self, lhs, rhs):
        rule = Rule(lhs, rhs)
        self.rules.append(rule)
