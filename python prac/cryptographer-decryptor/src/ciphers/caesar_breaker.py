from string import ascii_lowercase
import collections
import matplotlib.pylab as plt


class CaesarBypass:
    def __init__(self, text=""):
        self.text = text
        self.FrequencyAnalyzer_standard = self.FrequencyAnalyzer()
        self.FrequencyAnalyzer_standard.analyze_file("../data/Tartt_Donna_The_Goldfinch.txt")
        self.standard_frequency = self.FrequencyAnalyzer_standard.char_frequency
        self.FrequencyAnalyzer_cipher = self.FrequencyAnalyzer(text)
        self.cipher_frequency = self.FrequencyAnalyzer_cipher.char_frequency

    def get_all_possible(self):
        decrypted_texts = []
        for key in range(1, 26):
            decrypted_text = ""
            for char in self.text:
                if 'a' <= char <= 'z' or 'A' <= char <= 'Z':
                    char = char.lower()
                    shifted_char = chr((ord(char) - ord('a' if char.islower() else 'A') - key) % 26 + ord(
                        'a' if char.islower() else 'A'))
                    decrypted_text += shifted_char
                else:
                    decrypted_text += char
            decrypted_texts.append(decrypted_text)
        return decrypted_texts

    def caesar_breaker(self):
        best_shift = None
        best_score = float('-inf')

        for shift in range(26):
            score = 0
            for letter in self.standard_frequency:
                standard_count = self.standard_frequency[letter]
                shifted_letter = chr(((ord(letter) - ord('a') + shift) % 26) + ord('a'))
                cipher_count = self.cipher_frequency.get(shifted_letter, 0)
                score += standard_count * cipher_count
            if score > best_score:
                best_score = score
                best_shift = shift

        return best_shift

    class FrequencyAnalyzer:
        def __init__(self, text=None):
            self.char_frequency = collections.defaultdict(float)
            self.char_count = {}
            for char in ascii_lowercase:
                self.char_count[char] = 0
            self.load_text(text)

        def load_text(self, text):
            if text:
                self.analyze_text(text)

        def analyze_text(self, text):
            total_chars = 0
            for char in text:
                if 'a' <= char <= 'z' or 'A' <= char <= 'Z':
                    char = char.lower()
                    self.char_count[char] += 1
                    total_chars += 1
            for char, cnt in self.char_count.items():
                if total_chars != 0:
                    self.char_frequency[char] = cnt / total_chars
                else:
                    break

        def analyze_file(self, filename):
            with open(filename, 'r', encoding='utf-8') as file:
                text = file.read()
                self.analyze_text(text)

        def save_frequency(self, path):
            with open(path, 'w') as file:
                for char, freq in self.char_frequency.items():
                    file.write(f"{char}: {freq:.6f}\n")

        @staticmethod
        def load_frequency(path):
            char_frequency = collections.defaultdict(int)
            with open(path, 'r') as file:
                for line in file:
                    char, freq = line.strip().split(": ")
                    char_frequency[char] = int(freq)
            return char_frequency

        def plot_distribution(self):
            centers = range(1, len(self.char_frequency) + 1)
            plt.bar(centers, list(self.char_frequency.values()), align='center',
                    tick_label=list(self.char_frequency.keys()))
            plt.xlim(0, len(self.char_frequency) + 1)
            plt.show()
