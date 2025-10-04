class VigenereCipher:
    def __init__(self, text=""):
        self.text = text

    def encrypt(self, key):
        encrypted_text = ""
        key_length = len(key)
        for i, char in enumerate(self.text):
            shift = ord(key[i % key_length].lower()) - ord('a')
            encrypted_char = chr((ord(char) + shift) % 1114111)
            encrypted_text += encrypted_char
        return encrypted_text

    def decrypt(self, key):
        decrypted_text = ""
        key_length = len(key)
        for i, char in enumerate(self.text):
            shift = ord(key[i % key_length].lower()) - ord('a')
            decrypted_char = chr((ord(char) - shift) % 1114111)
            decrypted_text += decrypted_char
        return decrypted_text
