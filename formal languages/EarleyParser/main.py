from io_handler import IOHandler


def main():
    try:
        handler = IOHandler('input.txt')
        handler.read_input()
        handler.setup_parser()
        handler.process_words()
    except Exception as e:
        print(f"error: {e}")


if __name__ == "__main__":
    main()
