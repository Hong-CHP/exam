/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vbc.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hporta-c <hporta-c@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 15:02:30 by hporta-c          #+#    #+#             */
/*   Updated: 2025/10/15 15:18:18 by hporta-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VBC_H
# define VBC_H

# include <stdio.h>
# include <stdlib.h>
# include <ctype.h>

typedef struct s_parse
{
    char *input;
    int    pos;
}               t_parse;

typedef struct s_tree
{
    enum
    {
        ADD,
        MULTI,
        VAL,
    }   type;
    int val;
    struct s_tree *left;
    struct s_tree *right; 
}               t_tree;

t_tree  *parse_expr(t_parse *vbc);
t_tree  *parse_term(t_parse *vbc);
t_tree  *parse_factor(t_parse *vbc);

#endif