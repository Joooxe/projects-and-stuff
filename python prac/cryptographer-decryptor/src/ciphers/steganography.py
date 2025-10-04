from stegano import lsb


class Steganography:
    def __init__(self, path_to_img):
        self.path_to_img = path_to_img
        self.img = None

    def hide(self, text):
        img = lsb.hide(self.path_to_img, text)  # lsb - least significant bit
        self.img = img
        return img  # fun fact: it's Pillow image

    def reveal(self):
        message = lsb.reveal(self.path_to_img)
        return message


# source: https://git.sr.ht/~cedric/stegano/tree/master/item/stegano/lsb/lsb.py
