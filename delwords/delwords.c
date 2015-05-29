#include <helpers.h>

int count, deleted, offset, same, start;
char buf[4097];
char* word;

int main(int argc, char **args)
{
	int i, wordsize;

	if (argc != 2) {
		error("Use format: ./delwords <word>");
	}

	word = args[1];
	wordsize = strlen(word);

	for (count = read_(STDIN_FILENO, buf + offset, sizeof(buf) - offset); count > 0; count = read_(STDIN_FILENO, buf + offset, sizeof(buf) - offset)) {
		if (count == -1) {
			thiserror();
			return 1;
		}
		
		while (offset + same < count) {
			if (buf[offset + same] == word[same]) {
				++same;
				if (same == wordsize) {
					write_(STDOUT_FILENO, buf + start, offset - start);
					offset += same;
					same = 0;
					start = offset;
				}
			} else {
				if (same == 0) {
					++offset;
				} else {
					do {
						++offset;
						--same;
					} while (same > 0 && buf[offset] != word[0]);
				}
			}
		}
		
		write_(STDOUT_FILENO, buf + start, offset - start);
		for (i = 0; i < same; ++i) {
			buf[i] = buf[offset + i];
		}
		offset = same;
		start = 0;
	}

	return 0;
}
