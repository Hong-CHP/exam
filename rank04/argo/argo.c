/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   argo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hporta-c <hporta-c@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 17:06:44 by hporta-c          #+#    #+#             */
/*   Updated: 2025/10/15 21:24:28 by hporta-c         ###   ########.fr       */
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
    printf("ch: %c\n", c);
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

void skip_whitespace(FILE *stream)
{
    while (isspace(peek(stream)))
        getc(stream);
}

int parse_string(t_json *res, FILE *stream)
{
    char *buf;
    size_t c_read = 10;
    int c;
    int len = 0;
    
    skip_whitespace(stream);
    if (!accept(stream, '"'))
    {
        return (-1);
    }
    buf = malloc(c_read);
    if (!buf)
        return (-1);
    while ((c = getc(stream)) != EOF)
    {
        printf("ch: %c\n", c);
        if (c == '"')
            break ;
        if (len + 1 >= c_read)
        {
            c_read *= 2;
            char *tmp = realloc(buf, c_read);
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
    res->type = STRING;
    res->string = buf;
    return (1);
}

int parse_integer(t_json *res, FILE *stream)
{
    int c;
    int val = 0;
    int sign = 1;
    if (accept(stream, '+'))
        sign = 1;
    else if (accept(stream, '-'))
        sign = -1;
    if (!isdigit(peek(stream)))
        return (-1);
    while (isdigit(peek(stream)))
    {
        c = getc(stream);
        val = val * 10 + (c - '0');
    }
    res->type = INTEGER;
    res->integer = sign * val;
    return (1);
}

void    free_json(t_json js)
{
    size_t i;
    if (js.type == STRING)
        free(js.string);
    else if (js.type == MAP)
    {
        i = 0;
        while (i < js.map.size)
        {
            free(js.map.pair[i].key);
            free_json(js.map.pair[i].value);
            i++;
        }
        free(js.map.pair);
    }
}

void    destroy_data(t_pair *pair, size_t count)
{
    size_t i = 0;
    while (i < count)
    {
        free(pair[i].key);
        free_json(pair[i].value);
        i++;
    }       
}

int parse_map(t_json *res, FILE *stream)
{
    t_pair *pair;
    size_t n_pair = 4;
    size_t count = 0;
    t_json tmp_key;
    t_json tmp_val;
    
    if (!accept(stream, '{'))
        return (-1);
    if (accept(stream, '}'))
    {
        res->type = MAP;
        res->map.pair = NULL;
        res->map.size = 0;
        return (1);
    }
    pair = malloc(sizeof(t_pair) * n_pair);
    if (!pair)
        return (-1);
    while (1)
    {
        skip_whitespace(stream);
        if (parse_string(&tmp_key, stream) == -1)
        {
            destroy_data(pair, count);
            return (-1);
        }
        skip_whitespace(stream);
        if (!expect(stream, ':'))
        {
            destroy_data(pair, count);
            return (-1);
        }
        skip_whitespace(stream);
        if (parse_value(&tmp_val, stream) == -1)
        {
            printf("hehe\n");
            destroy_data(pair, count);
            return (-1);
        }
        if (count == n_pair)
        {
            n_pair *= 2;
            pair = realloc(pair, sizeof(t_pair) * n_pair);
            if (!pair)
                return (-1);
        }
        pair[count].key = tmp_key.string;
        pair[count].value = tmp_val;
        count++;
        skip_whitespace(stream);
        if (accept(stream, ','))
        {
            continue ;
        }
        break ;
    }
    skip_whitespace(stream);
    if (!expect(stream, '}'))
    {
        destroy_data(pair, count);
        return (-1);
    }
    res->type = MAP;
    res->map.pair = pair;
    res->map.size = count;
    return (1);
}

/* ---------- 根据下一个字符决定解析什么 ---------- */
int parse_value(t_json *res, FILE *stream)
{
    int c;
    while (isspace(c = peek(stream)))
        getc(stream);
    if (c == '"')
        return (parse_string(res, stream));
    if (c == '{')
        return (parse_map(res, stream));
    if (c == '+' || c == '-' || isdigit(c))
        return (parse_integer(res, stream));
    printf("unexpected token '%c'\n", c);
    return (-1);
}

/* ---------- 主入口 ---------- */
int argo(t_json *res, FILE *stream)
{
    if (parse_value(res, stream) == -1)
        return (-1);
    if (peek(stream) != EOF)
    {
        printf("unexpected extra data\n");
        free_json(*res);
        return (-1);
    }
    return (1);
}
