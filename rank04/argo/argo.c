/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   argo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hporta-c <hporta-c@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 08:36:46 by hporta-c          #+#    #+#             */
/*   Updated: 2025/10/22 14:44:25 by hporta-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "argo.h"

/* ---------- PARTIE FOURNIE PAR L’EXAMEN (argo.c initial) ---------- */

int	peek(FILE *stream)
{
	int	c = getc(stream);
	ungetc(c, stream);
	return c;
}

void	unexpected(FILE *stream)
{
	if (peek(stream) != EOF)
		printf("unexpected token '%c'\n", peek(stream));
	else
		printf("unexpected end of input\n");
}

int	accept(FILE *stream, char c)
{
	if (peek(stream) == c)
	{
		(void)getc(stream);
		return 1;
	}
	return 0;
}

int	expect(FILE *stream, char c)
{
	if (accept(stream, c))
		return 1;
	unexpected(stream);
	return 0;
}

int	skip_whitespace(FILE *file)
{
	int c = 0;
	while (isspace(peek(file)))
		c = getc(file);
	return (c);
}

void	free_json(t_json js)
{
	if (js.type == STRING)
		free(js.string);
	if (js.type == MAP)
	{
		int i = 0;
		while (i < js.map.size)
		{
			free(js.map.pair[i].key);
			free_json(js.map.pair[i].val);
			i++;
		}
		free(js.map.pair);
	}
}

void	destroy_pair(t_pair *pair, int size)
{
	int i = 0;
	while (i < size)
	{
		free(pair[i].key);
		free_json(pair[i].val);
		i++;
	}
	free(pair);
}

void print_js(t_json js, int indent)
{
	if (js.type == INTEGER)
		printf("%d", js.integer);
	else if (js.type == STRING)
		printf("\"%s\"", js.string);
	else if (js.type == MAP)
	{
		printf("{\n");
		for (int i = 0; i < js.map.size; i++)
		{
			for (int j = 0; j < indent; j++)
				printf("_");
			printf("\"%s\": ", js.map.pair[i].key);
			print_js(js.map.pair[i].val, indent + 2);
			if (i + 1 < js.map.size)
				printf(",\n");
		}
		printf("\n}");
	}
}