
// Written by ChatGPT

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>


// Reverse lookup table for some non-ASCII characters
// You can expand this table with more mappings as needed
static const struct {
	const char utf8[4];
	char ascii;
} reverse_lookup[] = {
	{ {0xCE,0xA1,0x00,0x00}, 'a' },
	{ {0xCE,0xA9,0x00,0x00}, 'e' },
	{ {0xCE,0xB3,0x00,0x00}, 'u' },
	{ {0xE2,0x80, 0x99,0x00}, '\'' },
	// Add more mappings here
};

char *utf8_to_ascii(const char *utf8_text)
{
	int j =0;

	char *ascii_text = malloc(1024); // Adjust the buffer size as needed

	if (!ascii_text) return NULL;

	ascii_text[0] = 0;

	for (int i = 0; utf8_text[i] != '\0';)
	{
		// Check if the character is non-ASCII (8th bit set)
		if ((utf8_text[i] & 0x80) == 0)
		{
			// ASCII character, copy as is
			ascii_text[j++] = utf8_text[i++];
		}
		else
		{
			int found_mapping = 0;

			// Try to find a reverse mapping in the lookup table
			for (int k = 0; k < sizeof(reverse_lookup) / sizeof(reverse_lookup[0]); k++)
			{
				if (strncmp(&utf8_text[i], (char*) reverse_lookup[k].utf8, strlen( (char*) reverse_lookup[k].utf8)) == 0)
				{
					// Found a reverse mapping, use the ASCII character
					ascii_text[j++] = reverse_lookup[k].ascii;
					i += strlen((char*)reverse_lookup[k].utf8);
					found_mapping = 1;
					break;
				}
			}

			// If no reverse mapping found, replace with '?'
			if (!found_mapping)
			{
				ascii_text[j++] = '?';

				// Advance to the next character by decoding UTF-8
				if ((utf8_text[i] & 0xE0) == 0xC0)
				{
					i += 2; // 2-byte UTF-8 character
				}
				else if ((utf8_text[i] & 0xF0) == 0xE0)
				{
					i += 3; // 3-byte UTF-8 character
				}
				else if ((utf8_text[i] & 0xF8) == 0xF0)
				{
					i += 4; // 4-byte UTF-8 character
				}
				else
				{
					i++; // Invalid UTF-8, advance by 1 to avoid infinite loop
				}
			}
		}
	}
	ascii_text[j] = '\0';

	return ascii_text;
}

int main(int args, char **arg)
{
//	const char *utf8_text = "Café Müller — Brötchen ☕";
	char *ascii_text;
	FILE *fd;

	if (args < 2) return 0;

	fd = fopen( arg[1] , "r" );
	
	if (fd)
	{
		char tmp[1000];

		while (fgets(tmp,sizeof(tmp),fd) != NULL)
		{
			ascii_text = utf8_to_ascii( tmp );
			if (ascii_text)
			{
				printf("%s",ascii_text);
				free(ascii_text);
			}
		}

		fclose(fd);
	}

	return 0;
}

