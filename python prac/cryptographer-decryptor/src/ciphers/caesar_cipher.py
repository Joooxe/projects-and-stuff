class CaesarCipher:
    def __init__(self, text=""):
        self.text = text

    def encrypt(self, key):
        for char in self.text:
            if not ('a' <= char <= 'z' or 'A' <= char <= 'Z'):
                return CaesarCipher.encrypt_all(self, key)
        return CaesarCipher.encrypt_english(self, key)

    def encrypt_english(self, key):
        encrypted_text = ""
        for char in self.text:
            if char.isalpha():
                shifted_char = chr(
                    (ord(char) - ord('a' if char.islower() else 'A') + key) % 26 + ord('a' if char.islower() else 'A'))
                encrypted_text += shifted_char
            else:
                encrypted_text += char
        return encrypted_text

    def encrypt_all(self, key):
        encrypted_text = ""
        for char in self.text:
            encrypted_char = chr((ord(char) + key) % 1114111)  # maximum Unicode code (lol)
            encrypted_text += encrypted_char
        return encrypted_text

    def decrypt(self, key):
        return self.encrypt(-key)

    def decrypt_english(self, key):
        return self.encrypt_english(-key)
