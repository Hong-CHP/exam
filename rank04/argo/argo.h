/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   argo.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hporta-c <hporta-c@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 17:05:36 by hporta-c          #+#    #+#             */
/*   Updated: 2025/10/15 20:17:08 by hporta-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ARGO_H
# define ARGO_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include <ctype.h>

/* ---------- 枚举：JSON 类型 ---------- */
typedef enum e_type
{
    INTEGER,
    STRING,
    MAP
} e_type;

typedef struct s_json t_json;
typedef struct s_pair t_pair;
typedef struct s_map t_map;

/* ---------- map 和 pair 结构 ---------- */

typedef struct s_map
{
    t_pair *pair;
    size_t size;
}               t_map;

/* ---------- json 结构 ---------- */
typedef struct s_json 
{
    e_type type;
    union {
        int    integer;
        char  *string;
        t_map    map;
    };
}              t_json;

typedef struct  s_pair 
{
    char *key;
    t_json value;
}               t_pair;

int argo(t_json *res, FILE *stream);
int parse_value(t_json *res, FILE *stream);
void    free_json(t_json js);

#endif