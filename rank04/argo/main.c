/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hporta-c <hporta-c@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 17:29:05 by hporta-c          #+#    #+#             */
/*   Updated: 2025/10/15 20:50:11 by hporta-c         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "argo.h"

void    print_json(t_json res)
{
    size_t i;
    if (res.type == INTEGER)
        printf("%d", res.integer);
    if (res.type == STRING)
        printf("\"%s\"", res.string);
    if (res.type == MAP)
    {
        printf("{\n");
        i = 0;
        while (i < res.map.size)
        {
            printf("\"%s\": ", res.map.pair[i].key);
            print_json(res.map.pair[i].value);
            if (i + 1 < res.map.size)
                printf(",");
            i++;
        }
        printf("}");
    }
}

int main(int argc, char *argv[])
{
    t_json res;
    if (argc != 2)
        return (1);
    FILE *f = fopen(argv[1], "r");
    if (!f)
        return (1);
    if (argo(&res, f) == -1)
    {
        printf("Failed to parse JSON\n");
        fclose(f);
        return (1);
    }
    fclose(f);
    print_json(res);
    printf("\n");
    free_json(res);
    return (0);
}