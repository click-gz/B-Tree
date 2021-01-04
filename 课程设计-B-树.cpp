#include <iostream>
#include<fstream>
#include<string>
#include<queue>
using namespace std;
//3阶B-TREE,每个节点最多保存2个关键字，每个节点最多三个孩子
typedef struct node {
    int Key_num;
    int Key[4];//*-*-*(0-1-2)
    struct node* parent;
    struct node* child[3];//0,1,2
}Node;
typedef struct Location {
    Node* tp;
    int i;//0,1,2
    int check;//0,1记录找没找到
}Location;

void Del_Elm(Node*&, Location*, int);
Location* Search(int num, Node* T) {
    Location* lo = new Location;
    lo->tp = new Node;
    int i ;
    for (i = 1; i <= T->Key_num; i++) {//key_num={1,2}
        if (num == T->Key[i]) {//找到了要查询的数据
            lo->i = i;
            lo->check = 1;
            break;
        }
        else if (num < T->Key[i]) {//没找到，去左边
            lo->i = i-1;
            lo->check = 0;
            break;
        }
    }
    if (i > T->Key_num) {//没找到，去右边(i=3/2),表示方向，并非是Key空间
        lo->i = i - 1;
        lo->check = 0;
    }
    return lo;
}

void Search_BT(int num, Node* BT, Location*& locat) {
    Node* temp_BT = BT;
    Node* pre = NULL;
    bool found = false;
    Location* temp_locat=new Location;
    while (temp_BT != NULL) {
        temp_locat = Search(num, temp_BT);
        if (temp_locat->check == 1) {//找到了
            locat->tp = temp_BT;
            locat->i = temp_locat->i;
            locat->check = 1;
            found = true;
            break;
        }
        else {//没有找到，就继续找
            pre = temp_BT;
            temp_BT = temp_BT->child[temp_locat->i];
        }
    }
    if (!found) {//没找到
        locat->check = 0;
        locat->i = temp_locat->i + 1;//调整到当前的Key空间
        locat->tp = pre;
    }
}
void Insert_Num(Node*& BT, int num, Location* locat) {
    Node* Target_InsertNode = locat->tp;
    if (Target_InsertNode->Key_num < 2 ){//可以直接写入数据
        if (locat->i == 1) {
            Target_InsertNode->Key[2] = Target_InsertNode->Key[1];
            Target_InsertNode->Key[1] = num;
            Target_InsertNode->Key_num++;
        }
        else {
            Target_InsertNode->Key[2] = num;
            Target_InsertNode->Key_num++;
        }
        
    }
    else {//该节点数据已满，需要调整
        int time = 0;
        Node* More_out = NULL;
        while (Target_InsertNode != NULL) {
            time++;
            int Elem[3];//找出需要上移的元素
            int k = 0;
            int i;
            for (i = 1; i <= Target_InsertNode->Key_num; i++) {
                if (num < Target_InsertNode->Key[i]) {
                    Elem[k] = num;
                    k++;
                    break;
                }
                Elem[k] = Target_InsertNode->Key[i];
                k++;
            }
            if (i <= Target_InsertNode->Key_num) {
                for (; i <= Target_InsertNode->Key_num; i++) {//将剩下的合并进去
                    Elem[k] = Target_InsertNode->Key[i];
                    k++;
                }
            }
            else {//只剩下num
                Elem[k] = num;
            }
            //Elem[1]
            int mid = 1;
            Target_InsertNode->Key_num = 1;
            Target_InsertNode->Key[1] = Elem[0];//左边的返回原来的
            Node* temp = new Node;
            temp->Key_num = 1;
            temp->Key[1] = Elem[2];//右边的新建一个保存起来
            for (int a = 0; a < 3; a++) {
                temp->child[a] = NULL;
            }
            if (time != 1) {//分裂的不是叶子，需要把下面的孩子分配给左右节点
                temp->child[0] = Target_InsertNode->child[2];
                Target_InsertNode->child[2]->parent = temp;
                Target_InsertNode->child[2] = NULL;
                temp->child[1] = More_out;
                More_out->parent = temp;
            }
            if (Target_InsertNode == BT) {//如果分裂的是根节点时
                Node* head = new Node;
                head->Key_num = 1;
                head->child[0] = Target_InsertNode;
                head->child[1] = temp;
                head->Key[1] = Elem[1];
                head->child[2] = NULL;
                Target_InsertNode->parent = head;
                temp->parent = head;
                head->parent = NULL;
                BT = head;
                return;
            }
            if (Target_InsertNode->parent->Key_num < 2) {//上面的节点不需要分裂时
                if (Elem[1] >= Target_InsertNode->parent->Key[1]) {
                    Target_InsertNode->parent->Key[2] = Elem[1];
                    Target_InsertNode->parent->Key_num += 1;
                    Target_InsertNode->parent->child[2] = temp;
                    temp->parent = Target_InsertNode->parent;
                    return;
                }
                else {
                    Target_InsertNode->parent->Key[2] = Target_InsertNode->parent->Key[1];
                    Target_InsertNode->parent->Key[1] = Elem[1];
                    Target_InsertNode->parent->child[2] = Target_InsertNode->parent->child[1];
                    Target_InsertNode->parent->child[1] = temp;
                    temp->parent = Target_InsertNode->parent;
                    Target_InsertNode->parent->Key_num += 1;
                    return;
                }
            }
            else {
                Target_InsertNode = Target_InsertNode->parent;
                num = Elem[1];
                More_out = temp;//将多分出来的右子树保存，之后需要用到
            }
        }
        
    }
}
void Init(Node*& BT) {
    fstream f;
    f.open("data.txt", ios::in);
    if (!f) {
        cout << "open error";
        exit(0);
    }
    string content;
    getline(f, content);
    f.close();
    int i = 0;
    while (true) {
        int num = 0;
        while (content[i] != ' '&& content[i]!='\0') {//获得数据
            num *= 10;
            num += content[i] - '0';
            i++;
        }
        if (BT->Key_num == 0) {//可以直接插入数据
            BT->Key[BT->Key_num + 1] = num;//存在1号位
            BT->Key_num += 1;
        }
        else {
            Location* locat = new Location;
            Search_BT(num, BT, locat);
            Insert_Num(BT,num,locat);
        }
        if (content[i] == '\0') {
            break;
        }
        i++;
    }
}
void Show(Node* BT) {
    if (BT->Key_num == 0) {
        cout << "B树已空" << endl;
        return;
    }
    Node* temp = BT;
    queue<Node*>Q;
    Q.push(temp);
    int time = 1;
    while(!Q.empty()) {
        int temp_time = 0;
        for (int j = 0; j < time; j++) {
            temp = Q.front();
            if (temp->parent == NULL)
                cout << "[]";
            else {
                cout << "[" << temp->parent->Key[1] << "]";
            }
            cout << '(';
            for (int i = 1; i <= temp->Key_num; i++) {
                cout << temp->Key[i] << " ";
            }
            cout << ")";
            if (time == 1)
                cout << endl;
            else
                cout << "    ";
            for (int i = 0; i < 3; i++) {
                if (temp->child[i] != NULL) {
                    Q.push(temp->child[i]);
                    temp_time++;
                }
            }
            Q.pop();
        }
        cout << endl;
        time = temp_time;
    }
}
void Adjust_BT(Node*& BT, Location* locat) {//调整B树
    Node* Target_Node = locat->tp;
    if (Target_Node->Key_num == 0 && Target_Node->parent == NULL && Target_Node->child[0] == NULL) {
        cout << "B树已空" << endl;
        free(BT);
        BT = new Node;
        for (int i = 0; i < 3; i++) {
            BT->child[i] = NULL;
        }
        BT->parent = NULL;
        BT->Key_num = 0;
        return;
    }
    if (Target_Node->Key_num >= 1) {
        return;
    }
    if (Target_Node->parent == NULL) {//根节点
        BT = Target_Node->child[0];
        BT->parent = 0;
        return;
    }
    if (Target_Node == Target_Node->parent->child[0]) {
        if (Target_Node->parent->child[1] != NULL) {
            if (Target_Node->parent->child[1] != NULL&&Target_Node->parent->child[1]->Key_num == 2) {//向右兄弟借一个
                Target_Node->Key_num++;
                Target_Node->Key[1] = Target_Node->parent->Key[1];
                Target_Node->parent->Key[1] = Target_Node->parent->child[1]->Key[1];
                Target_Node->parent->child[1]->Key[1] = Target_Node->parent->child[1]->Key[2];
                Target_Node->parent->child[1]->Key_num--;
                Node* tcr = Target_Node->parent->child[1]->child[0];
                Target_Node->parent->child[1]->child[0] = Target_Node->parent->child[1]->child[1];
                Target_Node->parent->child[1]->child[1] = Target_Node->parent->child[1]->child[2];
                Target_Node->parent->child[1]->child[2] = NULL;
                for (int i = 0; i < 3; i++) {
                    tcr->child[i] = NULL;
                }
                Target_Node->child[1] = tcr;
                tcr->parent = Target_Node;
                Location* lo = new Location;
                lo->tp = Target_Node->parent->child[1];
                lo->i = 1;
                Del_Elm(BT, lo, Target_Node->parent->child[1]->Key[1]);
            }
            else {//右兄弟没有多余的
                if (Target_Node->parent->Key_num == 2) {//双亲有多余的，拿一个下来
                    Target_Node->parent->child[1]->Key[2] = Target_Node->parent->child[1]->Key[1];
                    Target_Node->parent->child[1]->Key[1] = Target_Node->parent->Key[1];
                    Target_Node->parent->child[1]->Key_num++;
                    Target_Node->parent->Key_num--;
                    Target_Node->parent->Key[1] = Target_Node->parent->Key[2];
                    Node* tt = Target_Node->parent;
                    Target_Node->parent->child[1]->child[2] = Target_Node->parent->child[1]->child[1];
                    Target_Node->parent->child[1]->child[1] = Target_Node->parent->child[1]->child[0];
                    Target_Node->parent->child[1]->child[0] = Target_Node->child[0];
                    Target_Node->child[0]->parent = Target_Node->parent->child[1];
                    Target_Node->parent->child[0] = Target_Node->parent->child[1];
                    Target_Node->parent->child[1] = Target_Node->parent->child[2];
                    Target_Node->parent->child[2] = NULL;
                    Location* lo = new Location;
                    lo->tp = tt;
                    lo->i = 1;
                }
                else {//双亲没有多余的
                    Target_Node->parent->child[1]->Key[2] = Target_Node->parent->child[1]->Key[1];
                    Target_Node->parent->child[1]->Key[1] = Target_Node->parent->Key[1];
                    Target_Node->parent->child[1]->Key_num++;
                    Target_Node->parent->Key_num--;
                    Node* tt = Target_Node->parent;
                    Target_Node->parent->child[1]->child[2] = Target_Node->parent->child[1]->child[1];
                    Target_Node->parent->child[1]->child[1] = Target_Node->parent->child[1]->child[0];
                    Target_Node->parent->child[1]->child[0] = Target_Node->child[0];
                    Target_Node->child[0]->parent = Target_Node->parent->child[1];
                    Target_Node->parent->child[0] = Target_Node->parent->child[1];
                    Location* lo = new Location;
                    lo->tp = tt;
                    lo->i = 1;
                    Adjust_BT(BT, lo);
                }
            }
        }
        else {
            cout << "error TREE" << endl;
            exit(0);
        }
    }
    else  if(Target_Node==Target_Node->parent->child[1]){
        if (Target_Node->parent->child[0]!=NULL&&Target_Node->parent->child[0]->Key_num == 2) {//向左兄弟借一个
            Target_Node->Key_num++;
            Target_Node->Key[1] = Target_Node->parent->Key[1];
            Target_Node->parent->Key[1] = Target_Node->parent->child[0]->Key[2];
            Target_Node->parent->child[0]->Key_num--;
            Node* tcr = Target_Node->parent->child[0]->child[2];
            Target_Node->parent->child[0]->child[2] = NULL;
            for (int i = 0; i < 3; i++) {
                tcr->child[i] = NULL;
            }
            Target_Node->child[1] = tcr;
            tcr->parent = Target_Node;
        }
        else if (Target_Node->parent->child[2]!=NULL&&Target_Node->parent->child[2]->Key_num == 2) {//向右兄弟借一个
            Target_Node->Key_num++;
            Target_Node->Key[1] = Target_Node->parent->Key[2];
            Target_Node->parent->Key[2] = Target_Node->parent->child[2]->Key[1];
            Target_Node->parent->child[2]->Key[1] = Target_Node->parent->child[2]->Key[2];
            Target_Node->parent->child[2]->Key_num--;
            Node* tcr = Target_Node->parent->child[2]->child[0];
            Target_Node->parent->child[2]->child[0] = Target_Node->parent->child[2]->child[1];
            Target_Node->parent->child[2]->child[1] = Target_Node->parent->child[2]->child[2];
            Target_Node->parent->child[2]->child[2] = NULL;
            Target_Node->child[1] = tcr;
            tcr->parent = Target_Node;
            for (int i = 0; i < 3; i++) {
                tcr->child[i] = NULL;
            }
        }
        else {
            Node* tt = Target_Node->parent;
            Target_Node->parent->child[0]->Key_num++;
            Target_Node->parent->child[0]->Key[2] = Target_Node->parent->Key[1];
            Target_Node->parent->Key_num--;
            Target_Node->parent->child[0]->child[2] = Target_Node->child[0];
            Target_Node->child[0]->parent = Target_Node->parent->child[0];
            if (tt->Key_num == 2) {
                tt->Key[1] = tt->Key[2];
                tt->child[1] = tt->child[2];
                tt->child[2] = NULL;
            }
            Location* loact = new Location;
            locat->tp = tt;
            loact->i = 1;
            Adjust_BT(BT, locat);
        }
    }
    else {//是最右边的孩子，双亲有2个关键字
        if (Target_Node->parent->child[1] != NULL&&Target_Node->parent->child[1]->Key_num == 2) {//可以向左孩子借一个
            Target_Node->Key_num++;
            Target_Node->Key[1] = Target_Node->parent->Key[2];
            Target_Node->parent->Key[2] = Target_Node->parent->child[1]->Key[2];
            Target_Node->parent->child[1]->Key_num--;
            Node* temp = Target_Node->parent->child[1]->child[2];
            Target_Node->parent->child[1]->child[2] = NULL;
            for (int i = 0; i < 3; i++) {
                temp->child[i] = NULL;
            }
            Target_Node->child[1] = Target_Node->child[0];
            Target_Node->child[0] = temp;
            temp->parent = Target_Node;
        }
        else {
            Node* tt = Target_Node->parent;
            Target_Node->parent->child[1]->Key_num++;
            Target_Node->parent->child[1]->Key[2] == Target_Node->parent->Key[2];
            Target_Node->parent->Key_num--;
            Target_Node->parent -> child[1]->child[2] = Target_Node->child[0];
            Target_Node->child[0]->parent = Target_Node->parent->child[1];
            tt->child[2] = NULL;
            Location* locat = new Location;
            locat->tp = tt;
            locat->i = 2;
            Adjust_BT(BT, locat);
        }
    }
}
void delete_leaf(Node*& BT, Location* locat) {//删除底层的元素
    Node* Target_Node = locat->tp;
    if (Target_Node->Key_num == 1) {//只有一个元素，删除了会改变结构
        Target_Node->Key_num = 0;
        if (Target_Node->parent->child[1] == Target_Node) {
            if (Target_Node->parent->child[0] != NULL && Target_Node->parent->child[0]->Key_num == 2) {//向左兄弟借个
                int temp = Target_Node->parent->Key[1];
                Target_Node->parent->Key[1] = Target_Node->parent->child[0]->Key[Target_Node->parent->child[0]->Key_num];
                Target_Node->parent->child[0]->Key_num--;
                Target_Node->Key[1] = temp;
                Target_Node->Key_num++;
            }
            else if (Target_Node->parent->child[2] != NULL && Target_Node->parent->child[2]->Key_num == 2) {//向右兄弟借个
                int temp = Target_Node->parent->Key[2];
                Target_Node->parent->Key[2] = Target_Node->parent->child[2]->Key[Target_Node->parent->child[2]->Key_num];
                Target_Node->parent->child[2]->Key_num--;
                Target_Node->Key[1] = temp;
                Target_Node->Key_num++;
            }
            else {
                if (Target_Node->parent->Key_num == 2) {//双亲有两个关键字
                    if (Target_Node->parent->child[0] != NULL) {//左孩子存在，将双亲第一个关键字给他，然后将最右孩子变为中间孩子，双亲关键字数量变为1
                        Target_Node->parent->child[0]->Key_num++;
                        Target_Node->parent->child[0]->Key[2] = Target_Node->parent->Key[1];
                        Target_Node->parent->Key[1] = Target_Node->parent->Key[2];
                        Target_Node->parent->Key_num--;
                        if (Target_Node->parent->child[2] != NULL) {
                            Target_Node->parent->child[1] = Target_Node->parent->child[2];
                            Target_Node->parent->child[2] = NULL;
                        }
                        else {
                            cout << "error TREE" << endl;
                            exit(0);
                        }

                    }
                    else {
                        cout << "error TREE" << endl;
                        exit(0);
                    }
                }
                else {//双亲只有一个关键字,不会有child[2]
                    if (Target_Node->parent->child[0] != NULL) {//将此双亲节点与child[0]结合，化为叶节点
                        Target_Node->parent->child[0]->Key[2] = Target_Node->parent->Key[1];
                        Target_Node->parent->Key_num--;
                        Target_Node->parent->child[0]->Key_num++;
                        locat->tp = Target_Node->parent;//
                        Target_Node->parent->child[1] = NULL;
                        //调整BT；
                        Adjust_BT(BT, locat);
                    }
                    else {
                        cout << "error TREE" << endl;
                        exit(0);
                    }
                }

            }
        }
        else if (Target_Node->parent->child[0] == Target_Node) {
            if (Target_Node->parent->Key_num == 1) {//只有child[0],child[1]
                if (Target_Node->parent->child[1] != NULL) {
                    if (Target_Node->parent->child[1]->Key_num == 2) {//向右兄弟借一个
                        Target_Node->Key[1] = Target_Node->parent->Key[1];
                        Target_Node->Key_num++;
                        Target_Node->parent->Key[1] = Target_Node->parent->child[1]->Key[1];
                        Target_Node->parent->child[1]->Key[1] = Target_Node->parent->child[1]->Key[2];
                        Target_Node->parent->child[1]->Key_num--;
                    }
                    else {//将双亲与右兄弟结合，化为叶
                        Target_Node->parent->child[1]->Key[2] = Target_Node->parent->child[1]->Key[1];
                        Target_Node->parent->child[1]->Key[1] = Target_Node->parent->Key[1];
                        Target_Node->parent->Key_num--;
                        Target_Node->parent->child[1]->Key_num++;
                       // free(Target_Node->parent->child[0]);
                       // free(Target_Node->parent->child[1]);
                        Target_Node->parent->child[0] = Target_Node->parent->child[1];
                        Target_Node->parent->child[1] = NULL;
                        locat->tp = Target_Node->parent;
                        //调整树形结构
                        Adjust_BT(BT, locat);
                    }
                }
                else {
                    cout << "error TREE" << endl;
                    exit(0);
                }
            }
            else {//有child[0],child[1],child[2]
                if (Target_Node->parent->child[1] != NULL) {
                    if (Target_Node->parent->child[1]->Key_num == 2) {//向右兄弟借一个
                        Target_Node->Key[1] = Target_Node->parent->Key[1];
                        Target_Node->parent->Key[1] = Target_Node->parent->Key[2];
                        Target_Node->parent->Key[2] = Target_Node->parent->child[1]->Key[1];
                        Target_Node->parent->child[1]->Key[1] = Target_Node->parent->child[1]->Key[2];
                        Target_Node->parent->child[1]->Key_num--;
                    }
                    else {//将双亲一个关键字下放到孩子中，减少孩子数量
                        Target_Node->parent->child[1]->Key[2] = Target_Node->parent->child[1]->Key[1];
                        Target_Node->parent->child[1]->Key[1] = Target_Node->parent->Key[1];
                        Target_Node->parent->child[1]->Key_num++;
                        Target_Node->parent->Key[1] = Target_Node->parent->Key[2];
                        Target_Node->parent->Key_num--;
                        //free(Target_Node->parent->child[0]);
                        Target_Node->parent->child[0] = Target_Node->parent->child[1];
                        Target_Node->parent->child[1] = Target_Node->parent->child[2];
                        Target_Node->parent->child[2] = NULL;
                    }
                }
                else {
                    cout << "error TREE";
                    exit(0);
                }
            }
        }
        else {//作为最右边孩子
            if (Target_Node->parent->Key_num != 2) {
                cout << "error TREE" << endl;
                exit(0);
            }
            else {
                if (Target_Node->parent->child[1] != NULL) {
                    if (Target_Node->parent->child[1]->Key_num == 2) {//向左兄弟借一个
                        Target_Node->Key[1] = Target_Node->parent->Key[2];
                        Target_Node->parent->Key[2] = Target_Node->parent->child[1]->Key[2];
                        Target_Node->parent->child[1]->Key_num--;
                    }
                    else {//双亲减少一个关键字和孩子
                        Target_Node->parent->child[1]->Key[2] = Target_Node->parent->Key[2];
                        Target_Node->parent->child[1]->Key_num++;
                        Target_Node->parent->Key_num--;
                        Target_Node->parent->child[2] = NULL;

                    }
                }
                else {
                    cout << "error TREE" << endl;
                    exit(0);
                }
            }
        }
    }
    else {
        Target_Node->Key_num = 1;
        if (locat->i == 1) {
            Target_Node->Key[1] = Target_Node->Key[2];
        }
        return;
    }
}
Node* EXchange(Node*& BT, Location* locat) {//找到其右下子树中最小的的关键字，交换过来，并返回位置，删除该最小关键字
    Node* Target_Node = locat->tp->child[locat->i];
    while (Target_Node->child[0]!=NULL) {
        Target_Node = Target_Node->child[0];
    }
    locat->tp->Key[locat->i] = Target_Node->Key[1];
    return Target_Node;
}
void Del_Elm(Node*& BT, Location* locat,int num) {
    Node* Target_Node = locat->tp;
    while(Target_Node) {
        if (Target_Node->child[0] == NULL && Target_Node->child[1] == NULL && Target_Node->child[2] == NULL) {
            delete_leaf(BT, locat);
            break;
        }
        else {//不是叶
            Location* locatt = new Location;
            locatt->i = 1;
            locatt->tp= EXchange(BT, locat);
            delete_leaf(BT, locatt);
            break;
        }

    }
}
int main()
{
    Node* BT = new Node;
    for (int i = 0; i < 3; i++) {
        BT->child[i] = NULL;
    }
    BT->parent = NULL;
    BT->Key_num = 0;
    Init(BT);
    Show(BT);
    cout << "初始化完成" << endl;
    while (true) {
        cout << "请选择要操作的功能（1-3）：" << endl;
        cout << "1.查找，2.插入,3.删除,4.退出" << endl;
        cout << "请输入要操作的功能：";
        int choose = 0;
        cin >> choose;
        if (choose == 1) {
            int num;
            cin >> num;
            Location* locat = new Location;
            Search_BT(num, BT, locat);
            if (locat->check) {
                cout << "成功查找到：" << locat->tp->Key[locat->i] << endl;;
            }
            else {
                cout << "未查找到" << endl;
            }
        }
        else if (choose == 2) {
            cout << "请输入要插入的数据" << endl;
            int num;
            cin >> num;
            Location* locat = new Location;
            Search_BT(num, BT, locat);
            if (locat->check) {
                cout << "已经查询到该数据已存在" << endl;
            }
            else {
                Insert_Num(BT, num, locat);
            
                Show(BT);
            }
         }
        else if (choose == 3) {
            cout << "请输入要删除的数据:" << endl;
            int num;
            cin >> num;
            Location* locat = new Location;
            Search_BT(num, BT, locat);
            if (!locat->check) {
                cout << "未查询到数据" << endl;
            }
            else {
                Del_Elm(BT, locat, num);
                cout << "删除成功" << endl;
                Show(BT);
            }
        }
        else {
            break;
        }
    }
}
