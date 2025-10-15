/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vbc.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hporta-c <hporta-c@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 15:05:28 by hporta-c          #+#    #+#             */
/*   Updated: 2025/10/15 16:30:39 by hporta-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "vbc.h"

void    unexpected(char c)
{
    if (c)
       printf("Unexpected token '%c'\n", c);
    else
        printf("Unexpected end of input\n");
}

void destroy_tree(t_tree *node)
{
    if (!node)
        return ;
    destroy_tree(node->left);
    destroy_tree(node->right);
    free(node);
}

t_tree *new_tree_node(char val)
{
    t_tree *node = calloc(1, sizeof(t_tree));
    if (!node)
        return (NULL);
    if (isdigit(val))
    {
        node->type = VAL;
        node->val = val - '0';
    }
    else if (val == '+')
        node->type = ADD;
    else if (val == '*')
        node->type = MULTI;
    node->left = NULL;
    node->right = NULL;
    return (node);
}

t_tree  *parse_factor(t_parse *vbc)
{
    if (vbc->input[vbc->pos] == '(')
    {
        vbc->pos++;
        t_tree *node = parse_expr(vbc);
        if (vbc->input[vbc->pos] != ')')
        {
            unexpected(0);
            destroy_tree(node);
            return (NULL);    
        }
        vbc->pos++;
        return (node);
    }
    else if (isdigit(vbc->input[vbc->pos]))
    {
        t_tree *node = new_tree_node(vbc->input[vbc->pos]);
        vbc->pos++;
        return (node);
    }
    else
    {
        unexpected(vbc->input[vbc->pos]);
        return (NULL);
    }
}

t_tree  *parse_term(t_parse *vbc)
{
    t_tree *node = parse_factor(vbc);
    if (!node)
        return (NULL);
    while (vbc->input[vbc->pos] == '*')
    {
        vbc->pos++;
        t_tree *right = parse_factor(vbc);
        if (!right)
        {
            destroy_tree(node);
            return (NULL);
        }
        t_tree *root = new_tree_node('*');
        root->left = node;
        root->right = right;
        node = root;
    }
    return (node); 
}

t_tree  *parse_expr(t_parse *vbc)
{
    t_tree *node = parse_term(vbc);
    if (!node)
        return (NULL);
    while (vbc->input[vbc->pos] == '+')
    {
        vbc->pos++;
        t_tree *right = parse_term(vbc);
        if (!right)
        {
            destroy_tree(node);
            return (NULL);
        }
        t_tree *root = new_tree_node('+');
        root->left = node;
        root->right = right;
        node = root;
    }
    return (node);
}

int eval(t_tree *node)
{
    if (node->type == VAL)
        return (node->val);
    else if (node->type == ADD)
        return (eval(node->left) + eval(node->right));
    else if (node->type == MULTI)
        return (eval(node->left) * eval(node->right));
}

int main(int argc, char *argv[])
{
    t_parse vbc;
    t_tree *tree;
    
    if (argc != 2)
        return (1);
    if (!argv[1] || !*argv[1])
        return (1);
    vbc.input = argv[1];
    vbc.pos = 0;
    tree = parse_expr(&vbc);
    if (!tree || vbc.input[vbc.pos] != '\0')
    {
        if (tree)
            destroy_tree(tree);
        return (1);
    }
    printf("%d\n", eval(tree));
    destroy_tree(tree);
    return (0);
}