// VASILIS SYRPAS  	   AM:4174
// PANAGIOTIS BOULOTIS AM:4271
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int ucs_moves=0;
int astar_moves=0;
int N;
int minfinal = 9999;  //The cost of the best final state 
int visited[9999];    //An array that keeps every state that is already visited
int opened[9999];  
int visitedcost[9999]; 
int openedcost[9999];
int path[9999];       //The path from the final state to the start state

char * ucs( char * board, int cost); 
char * astar( char * board, int cost);
int final_state( char * board);                       //returns 1 -> final, 0 -> not final
char * fillboard();                                   //fills the board with the initial state
char * swap( char * board, int posd, int posb);       //swaps the dash (posd) with the chosen position (posb) of the board
int dash_position ( char * board);                    // returns the position of dash
int converter(char * board);                          //converts the board to a codified integer and returns it
int is_visited(char * boardy);                        //returns 0 -> not visited, 1 -> visited
int addto(int k, int * table);                        //adds the codyfied state of board to an array
int removefrom(int k, int * table);                   //removes the codified state from an array and then, left shifts so there will be no empty space in the array
char * reverse_conv(int state);                       //does the opposite of the converter
int findparent(char * board, int k, int cost);        //finds the path to the start state, for ucs
int findparent2(char * board, int k, int cost);       //finds the path to the start state, for astar
int couldbeparent(char * board, char * wannabeparent);//Returns the new cost -> we found a possible parent, returns -1 -> No parent
int printlist(int * alist);                           //for testing purposes
int heuristic(char * board);                          //computes the heuristic value of each state 


int main()
{
    int i,j;
    char *board;
    
    minfinal = 9999;

    for(i=0; i<9999; i++){
        visited[i]=-1;
        opened[i]=-1;
        visitedcost[i]=-1;
        openedcost[i]=-1;
        path[i]=-1;
        
    }

    printf("Give me the N value\n");
    scanf("%d",&N);
    
    board = (char*) malloc(((2*N)+1)* sizeof(char));
    
	board = fillboard();
	
	while(board==""){
	    printf("Something went wrong when filling board, please try again \n");
	    board = fillboard();
	}

	opened[0] = converter(board);
	openedcost[0] = 0;
	
    ucs(board,0);
    if(minfinal == 9999){
        printf("This cannot be solved\n");
        exit(0);
    }
    
    printf("\n The UCS path is: \n");
    
    i=0;
    while(path[i]!=-1){
        i++;
    }
    
    for(j=1; j<i; j++){
        printf("%s\n",reverse_conv(path[i-j-1]));
        
    }
    printf("\ncost %d with %d calls\n", minfinal, ucs_moves);
    
    for(i=0; i<9999; i++){
        visited[i]=-1;
        opened[i]=-1;
        visitedcost[i]=-1;
        openedcost[i]=-1;
        path[i]=-1;
        
    }
    minfinal = 9999;
    
    opened[0] = converter(board);
	openedcost[0] = heuristic(board);
    
    astar(board, 0);
    
    printf("\n The A* path is: \n");
    
    i=0;
    while(path[i]!=-1){
        i++;
    }
    
    for(j=1; j<i; j++){
        printf("%s\n",reverse_conv(path[i-j-1]));
        
    }
    printf("\ncost %d with %d calls\n", minfinal, astar_moves);
    
	free(board);
    return 0;
}

int final_state( char * board ){
    int i;
    int dash=0;
    
	//start with the left side of the state
    for(i=0;i<N;i++){
		//check if the A is on the left side and if so the state is not final
        if(board[i]=='A'){
            return 0;
        }
        if(board[i]=='-'){
            dash=1;
        }
    }
	//if the dash is before or in the middle and there is 'A' after that it's not final 
    if(dash==1){
        if(board[N]=='A'){
            return 0;
        }    
    }
	//if the last letter is not A the state	is not final
    if(board[2*N]!='A'){
        return 0;
    }
	//if none of the above are true, then it is a final state so return 1 to alert
    return 1;
    
}

char * fillboard(){
    int i;
    int len = (2*N)+1;
    int dash=0;
    int mcounter=0;
    int acounter=0;
    char * error="";

    char *board;
    board = (char*) malloc((len)* sizeof(char));

    printf("Give me the initial state, only correct characters are M,A,- \n");

     scanf("%s", board); //fill the board
 
    //check if the length is correct
    if(strlen(board)!=(2*N+1)){
        printf("Invalid input\n");
        return error;   
    }
    
    for(i=0;i<len;i++){
        //check if there is a invalid character other than 
        if(board[i]!='A' && board[i]!='M' && board[i]!='-'){
            printf("Invalid input, invalid character found\n");
            return error;
        }
        if(board[i]=='-'){
            dash++;
        }
        if(board[i]=='M'){
            mcounter++;
        }if(board[i]=='A'){
            acounter++;
        }
        //if there are more than one dashes then it's invalid   
        if(dash>1){
            printf("Invalid input only one '-' must be given\n");
            return error;
        }
    }
    //the M's or A's are more or less than N    
    if(mcounter!=N || acounter!=N){
        printf("Invalid input needs M's and A's amount equal to given N \n");
        return error;
    }
    //the board is filled correctly return it so it can be used by other functions  
    return board;   
}

int dash_position ( char * board){
	int i;
	
	for(i=0;i<(2*N+1);i++){
		if(board[i]=='-'){
			return i;
		}
	}
	//dash not found
	return -1;	
}

char * ucs( char * board, int cost ){
    ucs_moves++; // to check how many time the UCS algorithm has been called
    
    int i, moves, dashpos, mincost, minstate; 
    dashpos = dash_position(board); // finds the current position of '-'
    int j, costpos, replacement;
    replacement = 0;
    
    mincost = 999; 
    
    if(final_state(board)==1){
        costpos = removefrom(converter(board),opened);
        addto(converter(board),visited);
        addto(cost,visitedcost);
        
        while(openedcost[costpos]!=-1){
            openedcost[costpos] = openedcost[costpos+1];
            costpos++;
        }
        
        if(minfinal>cost){
            for(i=0; i<9999; i++){
                path[i]=-1;
            }
            minfinal = cost;
            path[0] = converter(board);
            findparent(board, 1, cost);//Create the shortest path
        }
      
        if(opened[0]!=-1){
            
            j=0;
            while(opened[j]!=-1){
                if(openedcost[j]<mincost){
                    mincost = openedcost[j];
                    minstate = opened[j];
                }
                j++;
            }
                
            ucs(reverse_conv(minstate), mincost);
        }
        return board;
        
    }
    
    //This is a way to find how many children states (moves) can be made
    if(dashpos<N+1){
        moves = dashpos+N+1;
    }else{
        moves = N+1 + ((2*N)+1 - dashpos);
    }
    
    //creates every possible child state and saves them to opened[]
    for(i=0; i<moves; i++){
        if((is_visited(swap(board,dashpos,i))==0)&&(abs(dashpos-i)<N+1)&&(i!=dashpos)){
            j=0;
            while(opened[j]!=-1){
                if(opened[j]==converter(swap(board,dashpos,i))){//The child already exists
                    replacement = 1;
                    if(openedcost[j]>cost+abs(dashpos-i)){//Checks if the new "version" of the child has lower cost
                        openedcost[j] = cost+abs(dashpos-i);//updates the lists with the lowest cost
                        opened[j]=converter(swap(board,dashpos,i));
                        
                    }
                }
                j++;
            }
            
            if(replacement!=1){//It's the first time we meet this child
                
                addto(converter(swap(board,dashpos,i)),opened);
                addto(cost + abs(dashpos-i),openedcost);
            }
                     
        }
    }
    
    costpos = removefrom(converter(board),opened);//The state is no longer opened, but visited
    addto(converter(board),visited);
    addto(cost,visitedcost);
    
    
    while(openedcost[costpos]!=-1){
        
        openedcost[costpos] = openedcost[costpos+1];//Shifts the costs to the left so there is no empty space
        costpos++;
    }
    
   
    //find the state with the smallest cost on the opened[]
    j=0;
    while(opened[j]!=-1){
        if((openedcost[j]<mincost)){
            mincost = openedcost[j];
            minstate = opened[j];
        }
        j++;
    }

    if(opened[0]!=-1){
        ucs(reverse_conv(minstate), mincost);//recursion with the optimal state
    }
    
	return board;
}

char * astar( char * board, int cost ){
    int i, moves, dashpos, mincost, minstate;
    dashpos = dash_position(board); // finds the current position of '-'
    int j, costpos, replacement;
    replacement = 0;
    astar_moves++;
    
    mincost = 999;
    
    if(final_state(board)==1){
        costpos = removefrom(converter(board),opened);
        addto(converter(board),visited);
        addto(cost,visitedcost);
        
        while(openedcost[costpos]!=-1){
            openedcost[costpos] = openedcost[costpos+1];
            costpos++;
        }
        
        if(minfinal>cost){
            for(i=0; i<9999; i++){
                path[i]=-1;
            }
            minfinal = cost;
            path[0] = converter(board);
            findparent2(board, 1, cost);//Create the shortest path
        }
        
        if(opened[0]!=-1){
            
            j=0;
            while(opened[j]!=-1){
                //Finding the best opened state
                if(openedcost[j]<mincost){
                    mincost = openedcost[j];
                    minstate = opened[j];
                }
                j++;
            }
            
            
            //same as UCS but taking heuristic cost into account
            astar(reverse_conv(minstate), mincost-heuristic(reverse_conv(minstate)));//recursion
        }
        return board;
        
    }
    
    //This is a way to find how many children states (moves) can be made
    if(dashpos<N+1){
        moves = dashpos+N+1;
    }else{
        moves = N+1 + ((2*N)+1 - dashpos);
    }
    
    
    for(i=0; i<moves; i++){
        if((is_visited(swap(board,dashpos,i))==0)&&(abs(dashpos-i)<N+1)&&(i!=dashpos)){
            j=0;
            while(opened[j]!=-1){
                if(opened[j]==converter(swap(board,dashpos,i))){
                    replacement = 1;//replacement doesn't necessarily happen
                    //Computes the total cost of the child state
                    if(openedcost[j] > cost + abs(dashpos - i) + heuristic(swap(board, dashpos, i))){
                        openedcost[j] = cost + abs(dashpos - i) + heuristic(swap(board, dashpos, i));
                        opened[j]=converter(swap(board,dashpos,i));
                        
                    }
                }
                j++;
            }
            
            if(replacement!=1){
                addto(converter(swap(board,dashpos,i)),opened);
                addto(cost + abs(dashpos - i) + heuristic(swap(board, dashpos, i)),openedcost);
            }
         
        }
    }
    
    costpos = removefrom(converter(board),opened);//The position of the cost at opened[]
    addto(converter(board),visited);
    addto(cost + heuristic(board),visitedcost);
    
    while(openedcost[costpos]!=-1){
        
        openedcost[costpos] = openedcost[costpos+1];
        costpos++;
    }
    
    //Searches in open[] for the state with the lowest cost    
    j=0;
    while(opened[j]!=-1){
        if((openedcost[j]<mincost)){
            mincost = openedcost[j];
            minstate = opened[j];
        }
        j++;
    }

    if(opened[0]!=-1){
        astar(reverse_conv(minstate), mincost-heuristic(reverse_conv(minstate)));
    }
    
	return board;
}

char * swap( char * board, int posd, int posb){
    char * tempboard;
    tempboard = (char*) malloc(((2*N)+1)* sizeof(char));
    int i;
    
    for(i=0; i<(2*N)+1; i++){
        tempboard[i] = board[i];
    }
    
    tempboard[posd] = board[posb];
    tempboard[posb] = board[posd];
    
    return tempboard;

}

int is_visited(char * boardy){
    int boardint = converter(boardy);
    int i = 0;
    int d;
	 
    while(visited[i]!=-1){
        if(boardint==visited[i]){
            return 1;
        }
        i++;
    }
    return 0;
    
}

int converter(char * board){
    char *intboard;
    int i, k;
    intboard = (char*) malloc(((2*N)+1)* sizeof(char));
    
    for(i=0; i<((2*N)+1); i++){
        
        if(board[i]=='M'){
            intboard[i] = '1';
        }
        
        if(board[i]=='A'){
            intboard[i] = '2';
        }
        
        if(board[i]=='-'){
            intboard[i] = '3';
        }
       
    }
    
    k = atoi(intboard);

    return k;
}

int addto(int k, int * table){
    int i=0;
    
    while(table[i]!=-1){
        i++;
    }
    
    table[i] = k;
    return 0;
}

int removefrom(int k, int * table){
    int j;
    int i=0;

    while(table[i]!=k){
        i++;
    }
    
    table[i] = table[i+1];
    j = i+1;
    
    while(table[j]!=-1){
        
        table[j] = table[j+1];
        j++;
    }
    
    return i;
    
}

char * reverse_conv(int state){
    char * st;
    int i;
    st = (char*) malloc(((2*N)+1)* sizeof(char));
    sprintf(st, "%d", state);
    
    for(i=0; i<((2*N)+1); i++){
        
        if(st[i]=='1'){
            st[i] = 'M';
        }
        
        if(st[i]=='2'){
            st[i] = 'A';
        }
        
        if(st[i]=='3'){
            st[i] = '-';
        }
    }
    
    return st;
}

int findparent(char * board, int k, int cost){
    //k is the position of path[] where the new parent will be added to
    int intboard = converter(board);
    int i=0;
    int newcost = 0;
    
    if(intboard==visited[0]){ //initial state found
        path[k] = intboard;
        return 0;
    }
    
    while(visited[i]!=-1){//Searching for papa
        
        newcost = couldbeparent(board, reverse_conv(visited[i]));
        if((newcost>-1)&&((cost-newcost)==visitedcost[i])){//a parent has been found
            path[k]=visited[i];
            findparent(reverse_conv(visited[i]), k+1, cost - newcost);//I used the parent to find the parent
            return 0;
        }
        
        i++;
    }
    
    return -1;
    
}

int findparent2(char * board, int k, int cost){
    int intboard = converter(board);
    int i=0;
    int newcost = 0;
    
    if(intboard==visited[0]){ //initial state found
        path[k] = intboard;
        return 0;
    }
    
    while(visited[i]!=-1){//Search for parent
        newcost = couldbeparent(board, reverse_conv(visited[i]));
        if((newcost>-1)&&(cost - heuristic(board) - newcost ==visitedcost[i] - heuristic(reverse_conv(visited[i])))){
            path[k]=visited[i];
            //Takes into account the heuristic costs
            findparent2(reverse_conv(visited[i]), k+1, cost - newcost - heuristic(board) + heuristic(reverse_conv(visited[i])) );
            return 0;
        }
        i++;
    }
    
    return 0;
}

int couldbeparent(char * board, char * wannabeparent){
    int i, var, dashboard, dashparent;
    dashboard = dash_position(board);
    dashparent = dash_position(wannabeparent);
    var = 0; //Counts how many parts are different
    
    /* For the state to be a parent it needs 4 things,
    1. To differ with the board in exactly 2 places
    2. To not have the dash at the same place
    3. Their distance shall not be greater than N
    4. Child's cost = parent's cost + dash relocation(-->it is checked in findparent())
    */
    
    for(i=0; i<(2*N+1); i++){
        
        if(board[i]!=wannabeparent[i]){
            var++;
            if((i!=dashboard)&&(i!=dashparent)){
                return -1;
            }
        }
    }
    
    if((var!=2)||(dashboard==dashparent)||abs(dashboard-dashparent)>N){
        return -1;
    }
    
    return abs(dashparent-dashboard);
}

int printlist(int * alist){
    int i;
    
    for(i=0; i<20; i++){
        if((alist[i]!=-1)&&(alist[i]>10000)){
            printf("%s ",reverse_conv(alist[i]));
        }else{
            printf("%d ",alist[i]);
        }
    }
    printf("\n");
    
    return 0;
}

int heuristic(char * board){ //explained in report.pdf
    int counter, i;
    
    counter = 0;
   
    for(i=0; i<N; i++){
        if(board[i]=='A'){
            counter = counter + N - i;
        }
    }
    
    for(i=N+1; i<2*N+1; i++){
        if(board[i]=='M'){
            counter = counter + i - N;
        }
    }
    
    if(board[2*N]=='-'){
        counter++;
    }
    
    return counter;
    
}








