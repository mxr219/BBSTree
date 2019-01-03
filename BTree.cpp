#include<stdio.h>
#include<stdlib.h>
#define OK 1 
#define ERROR 0
#define TRUE 1
#define FALSE 0
#define OVERFLOW -1
#define m 3  //B树阶数

typedef int Status;                //状态类型 
typedef int KeyType;               //关键字定义为int 
typedef struct Record{             //记录类型 
    KeyType key; 
}Record; 

typedef struct BTNode{
    int keynum;                 //关键字数目 
    KeyType key[m+1];           //关键字数组,0号单元未使用 
    struct BTNode *parent;     //双亲结点指针
    struct BTNode *ptr[m+1];   //孩子结点指针数组,0号单元未使用 
    Record *recptr[m+1];      //记录类型数组,0号未使用 
}BTNode, * BTree; 

typedef struct result{
    BTree pt;
    int i;
    int tag;
}result; 

int Search(BTree p ,int k){
    int i = 1;
    while(i<=p->keynum&&k>p->key[i]){
       i++;
    }
    return i;
}
void SearchBTree(BTree  t,int k,result &r){
    int  i = 0, found = 0;
    BTree p = t,q = NULL;
    while(p!=NULL&&0 == found ){
        i = Search(p,k);
        if(i<=p->keynum&&p->key[i]==k) found = 1;
        else {
             q = p;
             p = p->ptr[i-1];     
         }
    }
    if(1==found){
         r.pt = p;
         r.i = i;
         r.tag = 1;
    }else{
        r.pt = q;
        r.i = i;
        r.tag = 0; 
    }
} 

void split(BTree &q,int s,BTree &ap){
     int i,j,n = q->keynum;
     ap = (BTNode *)malloc(sizeof(BTNode));
     ap->ptr[0] = q->ptr[s];
     for(i=s+1,j=1;i<=n;i++,j++){   //将q的后半部分复制到ap中
          ap->key[j] = q->key[i];
          ap->ptr[j] = q->ptr[i];
     } 
     ap->keynum = n-s;             //给ap的keynum赋值 
     ap->parent = q->parent;       //给ap的parent赋值 
                             
     for(i=0;i<=n-s;i++){         //修改子节点的parent域; 
          if(ap->ptr[i]!=NULL){
                 ap->ptr[i]->parent = ap;
          }
     }
     q->keynum = s-1;
}

void newRoot(BTree &t,BTree q,BTree ap,int x){
     t = (BTNode *)malloc(sizeof(BTNode));
     t->keynum = 1;
     t->key[1] = x;
     t->parent = NULL;
     t->ptr[0] = q;     //t的左孩子 
     t->ptr[1] = ap;    //t的右孩子指针 
     if(q!=NULL)   q->parent = t;  
     if(NULL!=ap)   ap->parent = t;
     // t->recptr = ;  给记录类型数组赋值 
} 

void insert(BTree &q,int i,int x,BTree ap){
      int j, n = q->keynum;
      for(j = n;j>=i;j--){
           q->ptr[j+1] = q->ptr[j];
           q->key[j+1] = q->key[j];
      }
      q->key[i] = x;
      q->ptr[i] = ap;
      if(NULL!=ap) ap->parent = q;
      q->keynum++;
}

void InsertBTree(BTree &t,int k,BTree q,int i){
    int x,s,finished = 0,needNewRoot = 0;
    BTree ap;
    if(NULL == q) newRoot(t, NULL, NULL, k);    
    else{
       x = k;
       ap = NULL;
       while(0==needNewRoot && 0==finished){
             insert(q, i, x, ap);
          if(q->keynum<m) finished = 1;
          else{
                s = (m+1)/2;
                 split(q, s,ap);
                 x = q->key[s];
                 if(NULL!= q->parent){
                       q = q->parent;
                       i = Search( q ,x );
                }else{
                    needNewRoot = 1;
                }
          } 
       }
       if(1==needNewRoot)  newRoot(t, q, ap ,x);
    } 
}

Status insertBTree(BTree &t,int k){
     result r ;
     BTree q ;
     int i = 0;     
     SearchBTree( t,k,r);
     if(r.tag==1) return ERROR;
     q = r.pt;
     i = r.i;
     InsertBTree(t, k, q, i);
     return OK;
}

void Remove(BTree &p,int k){
     int i=0;
     for(i = k;i<p->keynum;i++){
          p->key[i] = p->key[i+1];  
          p->ptr[i] = p->ptr[i+1];
     }
     p->keynum--;
}

void Restore(BTree &t,BTree &p,int k){
     int s = (m-1)/2;
     BTree ppar = p->parent,pl,pr,pp;
     int i,next,last,j;
     if(NULL == ppar) {     
       t = p->ptr[0];
       if(NULL!=t)
       t->parent = NULL;
       free(p);
       return;
     }
     for(i=0;i<=ppar->keynum&&ppar->ptr[i]!=p;i++){     
       }
     next = i+1;
     last = i-1;
     if(i<ppar->keynum&&ppar->ptr[next]->keynum>s){    //右相邻兄弟有富余
         pr = ppar->ptr[next];
         p->key[++p->keynum] = ppar->key[next];       //pkey+1,
         if( NULL!=pr->ptr[0]){
              p->ptr[p->keynum] =  pr->ptr[0];        //若非最下层节点，则将pr第一个右孩子变成p的最后一个左孩子
              pr->ptr[0]->parent = p;                 //改变右孩子的双亲值
         }
                
         ppar->key[next] = pr->key[1];
         pr->ptr[0] = pr->ptr[1];
         for(j = 1;j<pr->keynum;j++){                  //将pr所有关键字左移
              pr->key[j] = pr->key[j+1];         
              pr->ptr[j] = pr->ptr[j+1];         
         }
         pr->keynum--; 
     }else if(i>0&&ppar->ptr[last]->keynum>s){       //左相邻兄弟有富余
         pl = ppar->ptr[last]; 
         p->keynum++;
         for(j = p->keynum;j>1;j--){
            p->key[j] = p->key[j-1];
            p->ptr[j] = p->ptr[j-1];
         }
         p->key[1] = ppar->key[i];
         if(NULL!=pl->ptr[pl->keynum]){
            p->ptr[0] = pl->ptr[pl->keynum];        //若非最下层节点，则将pl最后孩子变成p的第一个左孩子
            pl->ptr[pl->keynum]->parent = p;        //左孩子双亲
         }
         
         ppar->key[i] = pl->key[pl->keynum--];
     }else if(i<ppar->keynum){
         pr = ppar->ptr[next];
         p->key[++p->keynum] = ppar->key[next];
         
         if(pr->ptr[0]!=NULL){
           p->ptr[p->keynum] = pr->ptr[0];
           pr->ptr[0]->parent = p;
         }
       
         for(j=1;j<= pr->keynum;j++){
            p->key[++p->keynum] = pr->key[j];
            if(p->ptr[j]!=NULL){
              p->ptr[p->keynum] = pr->ptr[j];
              pr->ptr[j]->parent = p;
            }
         }
         pr->keynum = 0;
         free(pr);
         
         for(j = next;j<ppar->keynum;j++){
             ppar->key[j] = ppar->key[j+1];
             ppar->ptr[j] = ppar->ptr[j+1];
         }
         ppar->keynum--;
         if(ppar->keynum<s) Restore(t,ppar,k);
     }else{      
         pl = ppar->ptr[last];
         pl->key[++pl->keynum] = ppar->key[i];
        
         if(NULL!=p->ptr[0])  {
              pl->ptr[pl->keynum] = p->ptr[0];
              p->ptr[0]->parent = pl;         
         }
        
         for(j=1;j<= p->keynum;j++){
            pl->key[++p->keynum] = p->key[j];
            if(p->ptr[j]!=NULL){
             pl->ptr[p->keynum] = p->ptr[j];
             p->ptr[j]->parent = pl;
            }           
         }
         p->keynum = 0;
         free(p);
         
         for(j = i;j<ppar->keynum;j++){
             ppar->key[j] = ppar->key[j+1];
             ppar->ptr[j] = ppar->ptr[j+1];
         }
         ppar->keynum--;
         if(ppar->keynum<s) Restore(t,ppar,k);         
        }
}  
void Successor(BTree &p,int i){
     BTree ptemp,q;
     ptemp = p->ptr[i-1];
     q = ptemp;
     while(NULL!= ptemp) {
       q = ptemp;
       ptemp = ptemp->ptr[ptemp->keynum];       
     }
     p->key[i] = q->key[q->keynum];
     p = q;
}
void DeleteBTree(BTree &t,BTree &p,int i){
     if(NULL!=p->ptr[i]){
        Successor(p, i);
        DeleteBTree(t,p, p->keynum) ;
     }else{
        Remove(p,i);
        if(p->keynum<(m-1)/2){
           Restore(t,p,i);
        }
     }
}

Status deleteBTree(BTree &t,int k){
     result r ;
     BTree q ;
     int i = 0;     
     SearchBTree( t,k,r);
     if(r.tag==0) return ERROR;
     q = r.pt;
     i = r.i;
     DeleteBTree(t,q,i);
     return OK;
}
void printBTree(BTree t,int lev){
    if(NULL==t) return ;  
    int n = t->keynum;
    printBTree( t->ptr[n], lev+n);
    for(int j = t->keynum;j>0;j--){
          for(int k= 0;k<lev;k++){
            printf("   ");
          }
          printf("%2d \n",t->key[j]);
          printBTree( t->ptr[j-1], lev+n);
    }      
}

void travse(BTree t){
    if(NULL==t) return;
    travse(t->ptr[0]);
    int n = t->keynum;
    for(int i= 1;i<=n;i++){
       printf("%d ",t->key[i]);
       travse(t->ptr[i]);
    }
    return;
}
 
void destroy(BTree &t){
      int i =0;
     if(NULL==t ) return;     
     for(i;i<t->keynum;i++){
        destroy(t->ptr[i]);
     }
     free(t);
    // t = NULL;
}
void destr(BTree &t){
      while(t!=NULL)
      deleteBTree(t,t->key[1]);
}

void menu(){
	printf("======菜单栏=====\n");
	printf("1:插入新元素\n");
	printf("2:凹表输出\n");
	printf("3:中序遍历\n"); 
    printf("4:删除元素\n");
    printf("=================\n");
}
int main()
{
      BTree t = NULL ,q = NULL;
      int k ,i ;
      result r ;                                           
      int lev = 0;
      int key ;
      while(TRUE){
        int n = 0;
        menu(); 
        printf("请输入操作: ");
        scanf("%d",&n);
        if(n<0||n>5) continue;
        switch(n){        
          case 1:
             key = 0;
             scanf("%d",&key);
             insertBTree(t,key);
             printf("=================\n");
             printBTree(t,lev);
             printf("=================\n");
             break;
          case 2 :
             printf("=================\n");
             printBTree(t,lev);
             printf("=================\n");
             break;
          case 3: 
             travse(t);
             printf("\n");
             break;
          case 4:               
             scanf("%d",&key);
             deleteBTree(t,key );
             printf("=================\n");
             printBTree(t,lev);
             printf("=================\n");
              break; 
         case 5 :destr(t);
                 printf("B树已销毁\n");
                 break;
         default : 
               getchar();
               return 0;
        }     
     }    
      return 0;
     
}
