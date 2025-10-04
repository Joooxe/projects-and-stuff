from collections import defaultdict
from grammar import Grammar


class EarleyItem:
    def __init__(self, rule_index, dot, start):
        self.rule_index = rule_index
        self.dot = dot
        self.start = start

    def next_symbol(self, grammar):
        if self.dot < len(grammar.rules[self.rule_index].rhs):
            return grammar.rules[self.rule_index].rhs[self.dot]
        return None

    def is_complete(self, grammar):
        return self.dot >= len(grammar.rules[self.rule_index].rhs)

    def advance(self):
        return EarleyItem(self.rule_index, self.dot + 1, self.start)

    def __eq__(self, other):
        return (self.rule_index == other.rule_index and
                self.dot == other.dot and
                self.start == other.start)

    def __hash__(self):
        return hash((self.rule_index, self.dot, self.start))

    def __repr__(self):  # debug thing
        return f"[{self.rule_index}:{self.dot}:{self.start}]"


class EarleyParser:
    def __init__(self):
        self.grammar = Grammar()
        self.lhs_to_rule_indices = defaultdict(list)

    def fit(self, G):
        self.grammar = G
        self.lhs_to_rule_indices.clear()
        for idx, rule in enumerate(self.grammar.rules):
            self.lhs_to_rule_indices[rule.lhs].append(idx)

    def predict(self, item, pos, situations):
        if item.is_complete(self.grammar):
            return
        next_sym = item.next_symbol(self.grammar)
        if next_sym and next_sym in self.grammar.nonterminals:
            for rule_idx in self.lhs_to_rule_indices[next_sym]:
                new_item = EarleyItem(rule_idx, 0, pos)
                if new_item not in situations[pos]:
                    situations[pos].add(new_item)

    def complete(self, item, pos, situations):
        if not item.is_complete(self.grammar):
            return
        completed_rule = self.grammar.rules[item.rule_index]
        lhs = completed_rule.lhs
        for st in list(situations[item.start]):
            next_sym = st.next_symbol(self.grammar)
            if next_sym == lhs:
                new_item = st.advance()
                if new_item not in situations[pos]:
                    situations[pos].add(new_item)

    def scan(self, item, word, pos, situations):
        next_sym = item.next_symbol(self.grammar)
        if next_sym and next_sym in self.grammar.terminals:
            if pos < len(word) and word[pos] == next_sym:
                new_item = item.advance()
                if new_item not in situations[pos + 1]:
                    situations[pos + 1].add(new_item)

    def closure(self, situations, pos):
        is_changed = True
        while is_changed:
            is_changed = False
            for item in list(situations[pos]):
                size_before = len(situations[pos])
                self.predict(item, pos, situations)
                self.complete(item, pos, situations)
                size_after = len(situations[pos])
                if size_after > size_before:
                    is_changed = True

    def predict_word(self, word):
        situations = [set() for _ in range(len(word) + 1)]
        for rule_idx, rule in enumerate(self.grammar.rules):
            if rule.lhs == self.grammar.start_symbol:
                situations[0].add(EarleyItem(rule_idx, 0, 0))

        self.closure(situations, 0)
        for pos in range(len(word)):
            for item in situations[pos].copy():
                self.scan(item, word, pos, situations)
            self.closure(situations, pos + 1)

        for item in situations[len(word)]:
            if (self.grammar.rules[item.rule_index].lhs == self.grammar.start_symbol and
                    item.is_complete(self.grammar)):
                return True
        return False
