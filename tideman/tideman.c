#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool cycle(int loser , int comparator);
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    for(int i = 0 ; i < candidate_count ; i++)
    {
        if(strcmp(name , candidates[i]) == 0)
        {
            ranks[rank] = i ;
            return true ;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    int length = candidate_count ;
    for(int i = 0 ; i < length - 1 ; i++)
    {
        int x = ranks[i] ; // basically the identity of the candidate
        for(int j = i + 1 ; j < length ; j++)
        {
            int y = ranks[j] ;
            preferences[x][y] += 1 ;
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    int count = 0 ;
    for(int i = 0 ; i < candidate_count - 1 ; i++)
    {
        for(int j = i + 1 ; j < candidate_count ; j++)
        {

            int x = preferences[i][j] ; // how many prefered i over j
            int y = preferences[j][i] ; // how many prefreed j over i

            if(x > y)
            {
                pairs[count].winner = i ;
                pairs[count].loser = j ;
                count++ ;
            }
            else if (x < y)
            {
                pairs[count].winner = j ;
                pairs[count].loser = i ;
                count++ ;
            }
        }
        pair_count = count ;
    }
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    for(int i = 0 ; i < pair_count - 1 ; i++)
    {
        int x = preferences[pairs[i].winner][pairs[i].loser];
        int max = i ;
        for(int j = i + 1 ; j < pair_count ; j++)
        {
            int y = preferences[pairs[j].winner][pairs[j].loser];
            if(y > x)
            {
                max = j ;
            }

        }

        pair temp = pairs[i] ;
        pairs[i] = pairs[max] ;
        pairs[max] = temp ;
    }
    return;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    for(int i = 0 ; i < pair_count ; i++)
    {
        int currentWinner_Comparator = pairs[i].winner ;
        int currentLoser = pairs[i].loser ;

        if(cycle(currentLoser , currentWinner_Comparator) == false)
        {
            locked[currentWinner_Comparator][currentLoser] = true;
        }
    }
    return;
}

// Print the winner of the election
void print_winner(void)
{
    int x = 0 ;
    int cand = 0 ;
    for(int j = 0 ; j < candidate_count ; j++)
    {
        for(int i = 0 ; i < candidate_count ; i++)
        {
            if(locked[i][j] == true)
            {
                x = 1 ;
                break ;
            }
        }

        if(x != 1)
        {
            cand = j ;
            printf("%s\n" , candidates[j]) ;
            break ;
        }
        x = 0 ;
    }
    return;
}
bool cycle(int loser , int comparator)
{
    if(loser == comparator)
    {
        return true ;
    }

    for(int i = 0 ; i < candidate_count ; i++)
    {
        if(locked[loser][i] == true)
        {
            if(cycle(i,comparator) == true)
            {
                return true ;
            }
        }
    }
    return false ;
}