/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hporta-c <hporta-c@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 08:40:33 by hporta-c          #+#    #+#             */
/*   Updated: 2025/10/22 14:43:50 by hporta-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "argo.h"

int	parsing_string(FILE *file, t_json *js)
{
	int ch_read = 5;
	char *buf = NULL;
	char *tmp = NULL;
	int len = 0;
	int c;
	
	if (!accept(file, '"'))
		return (-1);
	buf = calloc(ch_read, sizeof(char));
	if (!buf)
		return (-1);
	while ((c = getc(file)) != '"' && c != EOF)
	{
		if (len + 1 >= ch_read)
		{
			ch_read *= 2;
			tmp = realloc(buf, ch_read);
			if (!tmp)
			{
				free(buf);
				return (-1);
			}
			buf = tmp;
		}
		buf[len++] = (char)c;
	}
	if (c != '"')
	{
		free(buf);
		return (-1);
	}
	buf[len] = '\0';
	js->type = STRING;
	js->string = buf;
	return (1);	
}

int parsing_interger(FILE *file, t_json *js)
{
	int c = 0;
	int nb = 0;
	int sign = 1;
	
	if (peek(file) != '+' && peek(file) != '-'
		&& !(peek(file) >= '0' && peek(file) <= '9'))
		return (-1);
	if (accept(file, '-'))
		sign = -1;
	else if (accept(file, '+'))
		sign = 1;
	while (isdigit(peek(file)) && (c = getc(file)) != EOF)
	{
		if (sign == 1)
		{
			if (nb > (2147483647 - (c - '0')) / 10)
			return (-1);
		}
		else
		{
			if (nb > (-2147483648 * sign - (c - '0') / 10))
				return (-1);
		}
		nb = nb * 10 + (c -'0');
	}
	js->integer = nb * sign;
	js->type = INTEGER;
	return (1);
}

int	parsing_map(FILE *file, t_json *js)
{
	int n_pair = 4;
	int count_pair = 0;
	t_pair *pair;
	t_json tmp_key;
	t_json tmp_val;
	
	if (!accept(file, '{'))
		return (-1);
	if (accept(file, '}'))
	{
		js->type = MAP;
		js->map.pair = NULL;
		js->map.size = 0;
		return (1);
	}
	pair = calloc(n_pair, sizeof(t_pair));
	if (!pair)
		return (-1);
	while (1)
	{
		skip_whitespace(file);
		if (parsing_string(file, &tmp_key) == -1)
		{
			destroy_pair(pair, count_pair);
			return (-1);
		}
		skip_whitespace(file);
		if (!expect(file, ':'))
		{
			destroy_pair(pair, count_pair);
			return (-1);
		}
		skip_whitespace(file);
		if (parse_value(file, &tmp_val) == -1)
		{
			destroy_pair(pair, count_pair);
			return (-1);
		}
		if (count_pair == n_pair)
		{
			n_pair *= 2;
			pair = realloc(pair, n_pair * sizeof(t_pair));
			if (!pair)
			{
				destroy_pair(pair, count_pair);
				return (-1);
			}
		}
		pair[count_pair].key = tmp_key.string;
		pair[count_pair].val = tmp_val;
		count_pair++;
		skip_whitespace(file);
		if (accept(file, ','))
			continue ;
		break ;
	}
	skip_whitespace(file);
	if (!expect(file, '}'))
	{
		destroy_pair(pair, count_pair);
		return (-1);
	}
	js->type = MAP;
	js->map.pair = pair;
	js->map.size = count_pair;
	return (1);
}

int parse_value(FILE *file, t_json *js)
{
	if (isdigit(peek(file)) || peek(file) == '+' || peek(file) == '-')
		return (parsing_interger(file, js));
	else if (peek(file) == '"')
		return (parsing_string(file, js));
	else if (peek(file) == '{')
		return (parsing_map(file, js));
    printf("unexpected token '%c'\n", getc(file));
	return (-1);
}

int argo(FILE *file, t_json *js)
{
	if (parse_value(file, js) == -1)
		return (-1);
	if (peek(file) != EOF)
	{
        printf("unexpected extra data\n");
        free_json(*js);
        return (-1);		
	}
	return (1);
}

int main(int argc, char **argv)
{
	FILE *file;
	t_json js;
	
	if (argc != 2)
		return (1);
	file = fopen(argv[1], "r");
	if (!file)
		return (1);
	if (argo(file, &js) == -1)
	{
		fclose(file);
		return (-1);
	}
	print_js(js, 2);
	printf("\n");
	free_json(js);
	fclose(file);
	return (0);
}
