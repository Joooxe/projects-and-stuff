import argparse
import sys
import os

from src.ciphers import vernam_cipher as vernam, caesar_cipher as caesar, steganography as stego, \
    caesar_breaker as breaker, vigenere_cipher as vigenere


class UI:
    @staticmethod
    def main(args):
        if len(sys.argv) <= 1:
            UI.interactive_mode()
        else:
            UI.console_mode(args)

    @staticmethod
    def console_mode(args):
        parser = argparse.ArgumentParser(description="Encryption Console")
        parser.add_argument("-i", action="store_true", help="Run in interactive mode")
        parser.add_argument("mode", choices=["encrypt", "decrypt", "break", "stego"], help="Mode of operation")
        parser.add_argument("--cipher", choices=["caesar", "vigenere", "vernam"], help="Cipher to use")
        args = parser.parse_args(args)

        if args.i:
            UI.interactive_mode()
            return

        if args.mode == "encrypt":
            UI.encrypt_message(args.cipher)
        elif args.mode == "decrypt":
            UI.decrypt_message(args.cipher)
        elif args.mode == "break":
            UI.break_caesar_cipher()
        elif args.mode == "stego":
            UI.steganography()
        else:
            print("Invalid mode. Please enter a valid mode.")
            parser.print_help()

    @staticmethod
    def interactive_mode():
        print("1. Encrypt a message")
        print("2. Decrypt a message")
        print("3. Break Caesar Cipher (English only)")
        print("4. Steganography (English only too)")
        choice = input("Enter your choice: ")

        if choice == "1":
            UI.encrypt_message()
        elif choice == "2":
            UI.decrypt_message()
        elif choice == "3":
            UI.break_caesar_cipher()
        elif choice == "4":
            UI.steganography()
        else:
            print("Invalid choice.")
            UI.restart()
            return
        UI.restart()

    @staticmethod
    def encrypt_message(cipher_type=None):
        if cipher_type is None:
            cipher_type = UI.get_type()
            if cipher_type is None:
                return

        message = UI.get_message()
        if message is None:
            return

        if cipher_type == "caesar":
            key = input("Enter the encryption key (shift number): ")
            cipher = caesar.CaesarCipher(message)
            encrypted_message = cipher.encrypt(int(key))
            print("Encrypted message:", encrypted_message)
            UI.save_result(encrypted_message, key, "Encrypted")
        elif cipher_type == "vigenere":
            key = input("Enter the encryption key: ")
            cipher = vigenere.VigenereCipher(message)
            encrypted_message = cipher.encrypt(key)
            print("Encrypted message:", encrypted_message)
            UI.save_result(encrypted_message, key, "Encrypted")
        elif cipher_type == "vernam":
            cipher = vernam.VernamCipher(message)
            key = vernam.VernamCipher.generate_key(len(message))
            encrypted_message = cipher.encrypt(key)
            print("Encrypted message:", encrypted_message)
            print("Key:", key)
            UI.save_result(encrypted_message, key, "Encrypted")

    @staticmethod
    def decrypt_message(cipher_type=None):
        if cipher_type is None:
            cipher_type = UI.get_type()
            if cipher_type is None:
                return

        message = UI.get_message()
        if message is None:
            return

        key = input("Enter the decryption key: ")
        if cipher_type == "caesar":
            cipher = caesar.CaesarCipher(message)
            decrypted_message = cipher.decrypt(int(key))
            print("Decrypted message:", decrypted_message)
            UI.save_result(decrypted_message, key, "Decrypted")
        elif cipher_type == "vigenere":
            cipher = vigenere.VigenereCipher(message)
            decrypted_message = cipher.decrypt(key)
            print("Decrypted message:", decrypted_message)
            UI.save_result(decrypted_message, key, "Decrypted")
        elif cipher_type == "vernam":
            cipher = vernam.VernamCipher(message)
            decrypted_message = cipher.decrypt(key)
            print("Decrypted message:", decrypted_message)
            UI.save_result(decrypted_message, key, "Decrypted")

    @staticmethod
    def break_caesar_cipher():
        message = UI.get_message()
        if message is None:
            return
        bypass = breaker.CaesarBypass(message)
        best_shift = bypass.caesar_breaker()
        if best_shift is not None:
            print("Best shift found:", best_shift)
            cipher = caesar.CaesarCipher(message)
            decrypted_message = cipher.decrypt(best_shift)
            print("Decrypted message:", decrypted_message)
            UI.save_result(decrypted_message, best_shift, "Unscrambled")
        else:
            print("Failed to break Caesar Cipher.")

        is_plot = input("Would you like to plot the cipher? (y/n): ")
        if is_plot.lower() == "y":
            bypass.FrequencyAnalyzer_cipher.plot_distribution()

        is_plot = input("Would you like to see standard plot? (y/n): ")
        if is_plot.lower() == "y":
            bypass.FrequencyAnalyzer_standard.plot_distribution()
            print("Btw you can load you own text to set standard frequency in frequency analyzer")
            print("No guarantee it will work tho")

    @staticmethod
    def steganography():
        print("1. Hide message in image")
        print("2. Reveal message in image")
        choice = input("Enter your choice: ")
        if choice == "1":
            path = input("Enter the path to the image: ")
            if not (os.path.exists(path)):
                print("Invalid path.")
                return
            message = UI.get_message()
            if message is None:
                return
            steganography = stego.Steganography(path)
            image = steganography.hide(message)
            image.save("../images/img_with_message.png")
            print("Done, message saved in img_with_message.png!")
        elif choice == "2":
            path = input("Enter the path to the image: ")
            if not (os.path.exists(path)):
                print("Invalid path.")
            steganography = stego.Steganography(path)
            message = steganography.reveal()
            print("Message revealed: ", message)
        else:
            print("Invalid choice.")

    @staticmethod
    def restart():
        choice = input("Would you like to restart the program? (y/n): ")
        if choice.lower() == "y":
            UI.main(sys.argv[1:])
        else:
            print("Bye!")

    @staticmethod
    def read_file(path):
        try:
            with open(path, "r") as file:
                text = file.read()
                return text
        except FileNotFoundError:
            return None

    @staticmethod
    def get_type():
        cipher_type = input("Choose a cipher (1. Caesar, 2. Vigenere, 3. Vernam): ")
        if cipher_type == "1":
            cipher_type = "caesar"
        elif cipher_type == "2":
            cipher_type = "vigenere"
        elif cipher_type == "3":
            cipher_type = "vernam"
        else:
            print("Invalid choice.")
            return None
        return cipher_type

    @staticmethod
    def get_message():
        message = None
        message_source = input("Choose message source (1. Console, 2. File): ")
        if message_source == "1":
            message = input("Enter the message to encrypt: ")
        elif message_source == "2":
            file_path = input("Enter the path to the file containing the message: ")
            message = UI.read_file(file_path)
            if message is None:
                print("Invalid path.")
                return None
        if message is None:
            print("Invalid choice.")
        return message

    @staticmethod
    def save_result(message, key, message_type):
        choice = input("Would you like to save the result? (y/n): ")
        if choice.lower() == "y":
            with open("../data/result.txt", "w") as file:
                file.write(f"{message_type} message: {message}\n")
                file.write(f"Key: {key}\n")
            print("Done, result saved in ../data/result.txt!")
