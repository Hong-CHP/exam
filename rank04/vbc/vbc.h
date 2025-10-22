/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vbc.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hporta-c <hporta-c@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 14:55:52 by hporta-c          #+#    #+#             */
/*   Updated: 2025/10/22 15:45:05 by hporta-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VBC_H
#define VBC_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef enum
{
	ADD,
	MULTI,
	VAL	
}	type;

typedef struct node
{
	type type;
	int val;
	struct node*	l;
	struct node*	r;
}	node;

int accept(char **s, char c);
void    unexpected(char c);
int expect(char **s, char c);
void    destroy_tree(node *n);
int eval_tree(node *tree);
node    *parse_expr(char **s);
node	*parse_term(char **s);
node	*parse_factor(char **s);


#endif