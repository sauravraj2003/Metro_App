#include<bits/stdc++.h>
using namespace std;

// --- Global Variables ---
vector<pair<int,int>> adj[100];
#define pb push_back
map<string,int> mp;
map<int,string> i2s;
int sz;
vector<string> keys;
vector<int> metro_map;

// --- Core Functions ---

void addEdge(string a, string b, int dis){
    int x = mp[a];
    int y = mp[b];
    adj[x].pb({y, dis});
    adj[y].pb({x, dis});
}

void Create_Metro_map(){
    keys.clear();
    mp.clear();
    i2s.clear();
    for(int i=0; i<100; i++) adj[i].clear();

    // --- Kanpur Metro Route (IIT to Central) ---
    keys.pb("IIT Kanpur");
    keys.pb("Kalyanpur");
    keys.pb("SPM Hospital");
    keys.pb("Vishwavidyalaya");
    keys.pb("Gurudev Chauraha");
    keys.pb("Geeta Nagar");
    keys.pb("Rawatpur");
    keys.pb("LLR Hospital");
    keys.pb("Moti Jheel");
    // Extension to Kanpur Central
    keys.pb("Chunniganj");
    keys.pb("Naveen Market");
    keys.pb("Bada Chauraha");
    keys.pb("Nayaganj");
    keys.pb("Kanpur Central");

    for(int i=0; i<keys.size(); i++){
        mp[keys[i]] = i;
        i2s[i] = keys[i];
    }
    sz = keys.size();

    // --- Edges (Approximate Distances) ---
    addEdge("IIT Kanpur", "Kalyanpur", 2);
    addEdge("Kalyanpur", "SPM Hospital", 1);
    addEdge("SPM Hospital", "Vishwavidyalaya", 2);
    addEdge("Vishwavidyalaya", "Gurudev Chauraha", 2);
    addEdge("Gurudev Chauraha", "Geeta Nagar", 2);
    addEdge("Geeta Nagar", "Rawatpur", 2);
    addEdge("Rawatpur", "LLR Hospital", 2);
    addEdge("LLR Hospital", "Moti Jheel", 1);
    
    // Extension Edges
    addEdge("Moti Jheel", "Chunniganj", 2);
    addEdge("Chunniganj", "Naveen Market", 1);
    addEdge("Naveen Market", "Bada Chauraha", 1);
    addEdge("Bada Chauraha", "Nayaganj", 2);
    addEdge("Nayaganj", "Kanpur Central", 1);
}

void displaystations(){
    cout << "\n--- KANPUR METRO STATIONS ---\n";
    for(int i=0; i<keys.size(); i++){
        cout << i+1 << ". " << keys[i] << endl;
    }
    cout << "-----------------------------\n";
}

void dfs(int src, int par, vector<bool>& vis){
    vis[src] = 1;
    metro_map.pb(src);
    for(auto x:adj[src]){
        if(!vis[x.first]){
            dfs(x.first, src, vis);
        }
    }
}

void displaymap(){
    metro_map.clear();
    vector<bool> vis(sz, false);
    dfs(0, -1, vis); 
    
    cout << "\n--- METRO ROUTE MAP ---\n";
    for(int i=0; i<metro_map.size(); i++){
        if(i != metro_map.size()-1)
            cout << i2s[metro_map[i]] << " <==> ";
        else
            cout << i2s[metro_map[i]];
    }
    cout << "\n-----------------------\n";
}

int shortestdistance(int src, int dest){
    vector<int> dist(sz, INT_MAX);
    dist[src] = 0;
    priority_queue< pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>> > pq;
    pq.push({0, src});
    
    while(!pq.empty()){
        auto x = pq.top();
        pq.pop();
        int dis = x.first;
        int u = x.second;
        
        if(dis > dist[u]) continue;

        for(auto y : adj[u]){
            int v = y.first;
            int weight = y.second;
            if(dist[v] > dist[u] + weight){
                dist[v] = dist[u] + weight;
                pq.push({dist[v], v});
            }
        }
    }
    return dist[dest];
}

void bfs(vector<int> parent[], int src){
    vector<int> dist(sz, INT_MAX);
    queue<int> q;
    q.push(src);
    dist[src] = 0;
    parent[src] = {-1};
    
    while(!q.empty()){
        int u = q.front();
        q.pop();
        for(auto x : adj[u]){
            int v = x.first;
            if(dist[v] > dist[u] + 1){
                dist[v] = dist[u] + 1;
                q.push(v);
                parent[v].clear();
                parent[v].pb(u);
            }
            else if(dist[v] == dist[u] + 1){
                parent[v].pb(u);
            }
        }
    }
}

void find_paths(vector<vector<int>>& paths, vector<int>& path, vector<int> parent[], int u){
    if(u == -1){
        paths.pb(path);
        return;
    }
    for(auto par : parent[u]){
        path.pb(u);
        find_paths(paths, path, parent, par);
        path.pop_back();
    }
}

void printpath(int src, int dest){
    vector<vector<int>> paths;
    vector<int> path;
    vector<int> parent[sz];
    
    bfs(parent, src);
    find_paths(paths, path, parent, dest);
    
    cout << "\n--- BEST ROUTE ---\n";
    for(auto v : paths){
        reverse(v.begin(), v.end());
        for(int i=0; i<v.size(); i++){
            cout << i2s[v[i]];
            if(i != v.size()-1) cout << " => ";
        }
        cout << endl;
        break; 
    }
    cout << "------------------\n";
}

int main(){
    Create_Metro_map();
    cout << "\n\t\t\t**** KANPUR METRO NAVIGATOR ****\n";
    
    while(true){
        cout << "\n\t\t\t\t~~ MENU ~~\n";
        cout << "1. LIST STATIONS\n";
        cout << "2. SHOW MAP\n";
        cout << "3. SHORTEST DISTANCE (KM)\n";
        cout << "4. SHORTEST TIME (MIN)\n";
        cout << "5. SHORTEST PATH (ROUTE)\n";
        cout << "6. EXIT\n";
        cout << "ENTER CHOICE: ";
        
        int choice;
        cin >> choice;
        
        if(choice == 1) displaystations();
        else if(choice == 2) displaymap();
        else if(choice == 3 || choice == 4){
            cout << "Enter Serial No. of Source and Destination: ";
            int x, y;
            cin >> x >> y;
            if(x < 1 || x > sz || y < 1 || y > sz) { cout << "Invalid!\n"; continue; }
            int p = shortestdistance(x-1, y-1);
            if(choice == 3) cout << "Distance: " << p << " KM\n";
            else cout << "Estimated Time: " << p << " Minutes\n";
        }
        else if(choice == 5){
            cout << "Enter Serial No. of Source and Destination: ";
            int x, y;
            cin >> x >> y;
            if(x < 1 || x > sz || y < 1 || y > sz) { cout << "Invalid!\n"; continue; }
            printpath(x-1, y-1);
        }
        else break;
    }
    return 0;
}
