/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hporta-c <hporta-c@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/22 15:00:38 by hporta-c          #+#    #+#             */
/*   Updated: 2025/10/22 16:44:36 by hporta-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include  "vbc.h"

node	*new_node(char val)
{
	node *new = calloc(1, sizeof(node));
	if (!new)
		return (NULL);
	if (val == '+')
		new->type = ADD;
	else if (val == '*')
		new->type = MULTI;
	else if (isdigit(val))
	{
		new->type = VAL;
		new->val = val - '0';
	}
	new->l = NULL;
	new->r = NULL;
	return (new);
}

node	*parse_factor(char **s)
{
	if (accept(s, '('))
	{
		// (*s)++;
		node *ret = parse_expr(s);
		if (!ret)
			return (NULL);
		if (!expect(s, ')'))
		{
			destroy_tree(ret);
			return (NULL);
		}
		// (*s)++;
		return (ret);
	}
	else if (isdigit(**s))
	{
		node *ret = new_node(**s);
		if (!ret)
			return (NULL);
		(*s)++;
		return (ret);
	}
	else
	{
		unexpected(**s);
		return (NULL);
	}
}

node	*parse_term(char **s)
{
	node *ret = parse_factor(s);
	if (!ret)
		return (NULL);
	while (**s == '*')
	{
		(*s)++;
		node *right = parse_factor(s);
		if (!right)
		{
			destroy_tree(ret);
        	return (NULL);
		}
		node *new = new_node('*');
		if (!new)
			return (NULL);
		new->l = ret;
		new->r = right;
		ret = new;
	}
    return (ret);
}

node    *parse_expr(char **s)
{
    node *ret = parse_term(s);
	if (!ret)
		return (NULL);
	while (**s == '+')
	{
		(*s)++;
		node *right = parse_term(s);
		if (!right)
		{
			destroy_tree(ret);
        	return (NULL);
		}
		node *new = new_node('+');
		if (!new)
			return (NULL);
		new->l = ret;
		new->r = right;
		ret = new;
	}
    return (ret);
}

int main(int argc, char **argv)
{
	node *tree;
	char *s;
	
	if (argc != 2)
		return (1);
	s = argv[1];
	tree = parse_expr(&s);
	if (!tree)
	{
		return (1);
	}
	if (*s != '\0')
	{
		unexpected(*s);
		destroy_tree(tree);
		return (1);
	}
	printf("%d\n", eval_tree(tree));
	destroy_tree(tree);
	return (0);
}
