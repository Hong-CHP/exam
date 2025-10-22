/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   argo.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hporta-c <hporta-c@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 08:38:50 by hporta-c          #+#    #+#             */
/*   Updated: 2025/10/22 11:40:38 by hporta-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ARGO_H
# define ARGO_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct s_pair t_pair;
typedef struct s_map t_map;
typedef struct s_json t_json;

typedef	enum
{
	STRING,
	INTEGER,
	MAP
}		type;

typedef struct s_map
{
	t_pair *pair;
	int 	size;
}				t_map;

typedef struct s_json
{
	union {
		char *string;
		int	integer;
		t_map map;
	};
	type	type;
	
}				t_json;

typedef struct s_pair
{
	char *key;
	t_json val;
}			t_pair;

int	peek(FILE *stream);
void	unexpected(FILE *stream);
int	accept(FILE *stream, char c);
int	expect(FILE *stream, char c);
int	skip_whitespace(FILE *file);
void	destroy_pair(t_pair *pair, int size);
void	free_json(t_json js);
void print_js(t_json js, int indent);
int	parse_value(FILE *file, t_json *js);

#endif