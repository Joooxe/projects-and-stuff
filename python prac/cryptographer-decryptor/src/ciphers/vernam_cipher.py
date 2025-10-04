from random import randint


class VernamCipher:
    def __init__(self, text=''):
        self.text = text

    @staticmethod
    def generate_key(length):
        new_key = ""
        for _ in range(length):
            new_key += chr(randint(0, 1114111))  # 65 to 90 for english one
        return new_key

    def encrypt(self, key):
        encrypted_text = ""
        for i, char in enumerate(self.text):
            encrypted_char = chr(ord(char) ^ ord(key[i]))  # % n (prefer 30-50 for readable synbols)
            encrypted_text += encrypted_char
        return encrypted_text

    def decrypt(self, key):
        return self.encrypt(key)
