from grammar import Grammar
from parser import EarleyParser


def test_simple():
    # S-> aS | b
    grammar = Grammar()
    grammar.nonterminals = {'S'}
    grammar.terminals = {'a', 'b'}
    grammar.start_symbol = 'S'
    grammar.add_rule('S', ['a', 'S'])
    grammar.add_rule('S', ['b'])

    parser = EarleyParser()
    parser.fit(grammar)
    assert parser.predict_word("a") is False
    assert parser.predict_word("") is False
    assert parser.predict_word("b") is True
    assert parser.predict_word("ab") is True
    assert parser.predict_word("aab") is True
    assert parser.predict_word("aaab") is True
    assert parser.predict_word("bb") is False


def test_epsilon():
    # S->
    grammar = Grammar()
    grammar.nonterminals = {'S'}
    grammar.terminals = set()
    grammar.start_symbol = 'S'
    grammar.add_rule('S', [])

    parser = EarleyParser()
    parser.fit(grammar)

    assert parser.predict_word("") is True
    assert parser.predict_word("a") is False


def test_random():
    grammar = Grammar()
    grammar.nonterminals = {'S', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I'}
    grammar.terminals = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'}
    grammar.start_symbol = 'S'
    grammar.add_rule('S', ['A', 'B'])
    grammar.add_rule('S', ['C', 'D'])
    grammar.add_rule('A', ['a', 'A'])
    grammar.add_rule('A', [])
    grammar.add_rule('B', ['b', 'B'])
    grammar.add_rule('B', ['c'])
    grammar.add_rule('C', ['d', 'C'])
    grammar.add_rule('C', ['e'])
    grammar.add_rule('D', ['f', 'D'])
    grammar.add_rule('D', ['g'])
    grammar.add_rule('E', ['h', 'E'])
    grammar.add_rule('E', [])
    grammar.add_rule('F', ['a', 'B'])
    grammar.add_rule('G', ['A', 'F'])
    grammar.add_rule('H', ['G', 'H'])
    grammar.add_rule('I', ['h'])

    parser = EarleyParser()
    parser.fit(grammar)

    test_words = {
        "c": True,
        "ac": True,
        "aabc": True,
        "aaabbbc": True,
        "eg": True,
        "deg": True,
        "defg": True,
        "ddefg": True,
        "deffg": True,
        "dddeffg": True,
        "fg": False,
        "h": False,
        "hh": False,
        "hhh": False,
        "ahf": False,
        "aab": False,
        "abcd": False,
        "aabbbcc": False,
        "dddeeefffg": False,
        "aaabbbcccddd": False,
        "ahafh": False
    }

    for word, expected in test_words.items():
        assert parser.predict_word(word) is expected, f"'{word}' expected to be {expected}"


def test_double():
    # S->SS | a
    grammar = Grammar()
    grammar.nonterminals = {'S'}
    grammar.terminals = {'a'}
    grammar.start_symbol = 'S'
    grammar.add_rule('S', ['S', 'S'])
    grammar.add_rule('S', ['a'])

    parser = EarleyParser()
    parser.fit(grammar)

    assert parser.predict_word("") is False
    assert parser.predict_word("a") is True
    assert parser.predict_word("ab") is False
    assert parser.predict_word("aaaa") is True
    assert parser.predict_word("aaaaaa") is True


def test_empty():
    grammar = Grammar()
    grammar.nonterminals = {'S'}
    grammar.terminals = {'a'}
    grammar.start_symbol = 'S'
    parser = EarleyParser()
    parser.fit(grammar)

    assert parser.predict_word("") is False
    assert parser.predict_word("a") is False


def test_recursion():
    # A -> AB | a
    # B -> BA | b
    grammar = Grammar()
    grammar.nonterminals = {'A', 'B'}
    grammar.terminals = {'a', 'b'}
    grammar.start_symbol = 'A'
    grammar.add_rule('A', ['A', 'B'])
    grammar.add_rule('A', ['a'])
    grammar.add_rule('B', ['A'])
    grammar.add_rule('B', ['b'])

    parser = EarleyParser()
    parser.fit(grammar)

    assert parser.predict_word("a") is True
    assert parser.predict_word("b") is False
    assert parser.predict_word("abb") is True
    assert parser.predict_word("baa") is False
    assert parser.predict_word("abba") is True
    assert parser.predict_word("abbb") is True
    assert parser.predict_word("abab") is True
    assert parser.predict_word("baa") is False
    assert parser.predict_word("aaa") is True
    assert parser.predict_word("bbb") is False


def test_substr():
    # S -> abcdefgh
    grammar = Grammar()
    grammar.nonterminals = {'S'}
    grammar.terminals = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'}
    grammar.start_symbol = 'S'
    grammar.add_rule('S', list("abcdefgh"))

    parser = EarleyParser()
    parser.fit(grammar)

    assert parser.predict_word("abcdefgh") is True
    assert parser.predict_word("abcdefg") is False
    assert parser.predict_word("abcde") is False
    assert parser.predict_word("abcdefghh") is False
    assert parser.predict_word("a") is False
    assert parser.predict_word("bcdefghh") is False
    assert parser.predict_word("bcdefgh") is False
    assert parser.predict_word("") is False


def test_not_reachable():
    # S->x
    # A->y
    # B->z
    grammar = Grammar()
    grammar.nonterminals = {'S', 'A', 'B'}
    grammar.terminals = {'x', 'y', 'z'}
    grammar.start_symbol = 'S'
    grammar.add_rule('S', ['x'])
    grammar.add_rule('A', ['y'])
    grammar.add_rule('B', ['z'])

    parser = EarleyParser()
    parser.fit(grammar)

    assert parser.predict_word("x") is True
    assert parser.predict_word("y") is False
    assert parser.predict_word("z") is False
    assert parser.predict_word("xy") is False
    assert parser.predict_word("") is False


def test_not_reachable2():
    # S->a | bb
    grammar = Grammar()
    grammar.nonterminals = {'S'}
    grammar.terminals = {'a', 'b', 'c'}
    grammar.start_symbol = 'S'
    grammar.add_rule('S', ['a'])
    grammar.add_rule('S', ['b', 'b'])

    parser = EarleyParser()
    parser.fit(grammar)

    assert parser.predict_word("a") is True
    assert parser.predict_word("bb") is True
    assert parser.predict_word("b") is False
    assert parser.predict_word("c") is False
    assert parser.predict_word("ccc") is False


def test_exact_len():
    # S->XY
    # X->x|y
    # Y->x|y
    grammar = Grammar()
    grammar.nonterminals = {'S', 'X', 'Y'}
    grammar.terminals = {'x', 'y'}
    grammar.start_symbol = 'S'
    grammar.add_rule('S', ['X', 'Y'])
    grammar.add_rule('X', ['x'])
    grammar.add_rule('X', ['y'])
    grammar.add_rule('Y', ['x'])
    grammar.add_rule('Y', ['y'])

    parser = EarleyParser()
    parser.fit(grammar)

    assert parser.predict_word("xx") is True
    assert parser.predict_word("xy") is True
    assert parser.predict_word("yx") is True
    assert parser.predict_word("yy") is True
    assert parser.predict_word("x") is False
    assert parser.predict_word("y") is False
    assert parser.predict_word("xxx") is False
    assert parser.predict_word("yyy") is False


def debug_test():
    # S->bbb
    grammar = Grammar()
    grammar.nonterminals = {'S'}
    grammar.terminals = {'b'}
    grammar.start_symbol = 'S'
    grammar.add_rule('S', ['b', 'b', 'b'])

    parser = EarleyParser()
    parser.fit(grammar)

    assert parser.predict_word("") is False
    assert parser.predict_word("b") is False
    assert parser.predict_word("bb") is False
    assert parser.predict_word("bbb") is True
    assert parser.predict_word("bbbb") is False


def debug_test2():
    # S->AB
    # A->aA
    # B->bB
    # B->c
    grammar = Grammar()
    grammar.nonterminals = {'S', 'A', 'B'}
    grammar.terminals = {'a', 'b', 'c'}
    grammar.start_symbol = 'S'
    grammar.add_rule('S', ['A', 'B'])
    grammar.add_rule('A', ['a', 'A'])
    grammar.add_rule('A', [])
    grammar.add_rule('B', ['b', 'B'])
    grammar.add_rule('B', ['c'])

    parser = EarleyParser()
    parser.fit(grammar)

    assert parser.predict_word("aaaaabbbbc") is True


def debug_test3():
    # S->AB
    # A->
    # B->c
    grammar = Grammar()
    grammar.nonterminals = {'S', 'A', 'B'}
    grammar.terminals = {'a', 'b', 'c'}
    grammar.start_symbol = 'S'
    grammar.add_rule('S', ['A', 'B'])
    grammar.add_rule('A', [])
    grammar.add_rule('B', ['c'])

    parser = EarleyParser()
    parser.fit(grammar)

    assert parser.predict_word("c") is True


def run_tests():
    test_simple()
    test_epsilon()
    test_double()
    test_empty()
    test_recursion()
    test_substr()
    test_not_reachable()
    test_not_reachable2()
    test_exact_len()
    test_random()
    debug_test()
    debug_test2()
    debug_test3()
    print("Let's GOOOOOOOOOOOOOOOOOOO")


if __name__ == "__main__":
    run_tests()
