from collections import deque
from grammar import Grammar
from parser import EarleyParser


class IOHandler:
    def __init__(self, input_file: str = 'input.txt'):
        self.input_file = input_file
        self.grammar = Grammar()
        self.words = []
        self.parser = EarleyParser()

    def read_input(self):
        with open(self.input_file, 'r', encoding='utf-8') as f:
            lines = deque(line.strip() for line in f if line)

        if not lines:
            raise "Input is empty"

        parts = lines.popleft().split()
        if len(parts) != 3:
            raise "First line should be 3 ints"
        n, sigma, p = map(int, parts)

        if len(lines) < 1:
            raise "No nonterminals ? :("
        nonterminals = list(lines.popleft())
        if len(nonterminals) != n:
            raise "Nonterminals not equal n"

        self.grammar.nonterminals = set(nonterminals)

        if len(lines) < 1:
            raise "No terminals ? :("
        terminals = list(lines.popleft())
        if len(terminals) != sigma:
            raise "Nonterminals not equal alphabet (sigma)"

        self.grammar.terminals = set(terminals)

        if len(lines) < p:
            raise "Not enough rules"
        for _ in range(p):
            rule_line = lines.popleft()
            if '->' not in rule_line:
                raise f"Wrong format: {rule_line}"
            lhs, rhs = rule_line.split('->', 1)
            lhs = lhs.strip()
            rhs = rhs.strip()
            if rhs == '':
                self.grammar.add_rule(lhs, [])
            else:
                self.grammar.add_rule(lhs, list(rhs))

        if len(lines) < 1:
            raise "No starting symbol ? :("
        start_symbol = lines.popleft()
        self.grammar.start_symbol = start_symbol

        m_line = lines.popleft()
        m = int(m_line)

        if len(lines) < m:
            raise "Not enough words given"
        self.words = [lines.popleft() for _ in range(m)]

    def setup_parser(self):
        self.parser.fit(self.grammar)

    def process_words(self):
        results = []
        for word in self.words:
            print("Yes") if self.parser.predict_word(word) else print("No")
        return results
