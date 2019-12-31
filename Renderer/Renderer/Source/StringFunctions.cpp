#pragma once

bool stringEqual(char* string1, char* string2, int length)
{
	for (int i = 0; i < length; i++)
	{
		if (string1[i] != string2[i])
		{
			return false;
		}
	}
	return true;
}

float parseFloat(char* string)
{
	int character = 0;
	float result = 0.0f;
	if (string[0] == '-')
	{
		int counter = 0;
		character += 1;
		while ((*(string + counter + 2)) <= 57 && (*(string + counter + 2)) >= 48)
		{
			counter += 1;
		}
		while ((*(string + character)) >= 48 && (*(string + character)) <= 57)
		{
			result -= ((*(string + character)) - 48) * pow(10.0f, counter);
			counter -= 1;
			if ((*(string + character + 1)) == '.')
			{
				character += 1;
			}
			if ((*(string + character + 1)) == 'e')
			{
				int exponent = (int)parseFloat(string + character + 2);
				result = result * pow(10.0f, exponent);
			}
			character += 1;
		}
		return result;
	}
	else if (string[0] >= 48 && string[0] <= 57)
	{
		int counter = 0;
		while ((*(string + counter + 1)) <= 57 && (*(string + counter + 1)) >= 48)
		{
			counter += 1;
		}
		while ((*(string + character)) >= 48 && (*(string + character)) <= 57)
		{
			result += ((*(string + character)) - 48) * pow(10.0f, counter);
			counter -= 1;
			if ((*(string + character + 1)) == '.')
			{
				character += 1;
			}
			if ((*(string + character + 1)) == 'e')
			{
				int exponent = (int)parseFloat(string + character + 2);
				result = result * pow(10.0f, exponent);
			}
			character += 1;
		}
		return result;
	}
	return 0;
}