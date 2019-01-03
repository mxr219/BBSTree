#include<stdio.h>
#include<stdlib.h>
#define OK 1 
#define ERROR 0
#define TRUE 1
#define FALSE 0
#define OVERFLOW -1
#define m 3  //B������

typedef int Status;                //״̬���� 
typedef int KeyType;               //�ؼ��ֶ���Ϊint 
typedef struct Record{             //��¼���� 
    KeyType key; 
}Record; 

typedef struct BTNode{
    int keynum;                 //�ؼ�����Ŀ 
    KeyType key[m+1];           //�ؼ�������,0�ŵ�Ԫδʹ�� 
    struct BTNode *parent;     //˫�׽��ָ��
    struct BTNode *ptr[m+1];   //���ӽ��ָ������,0�ŵ�Ԫδʹ�� 
    Record *recptr[m+1];      //��¼��������,0��δʹ�� 
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
     for(i=s+1,j=1;i<=n;i++,j++){   //��q�ĺ�벿�ָ��Ƶ�ap��
          ap->key[j] = q->key[i];
          ap->ptr[j] = q->ptr[i];
     } 
     ap->keynum = n-s;             //��ap��keynum��ֵ 
     ap->parent = q->parent;       //��ap��parent��ֵ 
                             
     for(i=0;i<=n-s;i++){         //�޸��ӽڵ��parent��; 
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
     t->ptr[0] = q;     //t������ 
     t->ptr[1] = ap;    //t���Һ���ָ�� 
     if(q!=NULL)   q->parent = t;  
     if(NULL!=ap)   ap->parent = t;
     // t->recptr = ;  ����¼�������鸳ֵ 
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
     if(i<ppar->keynum&&ppar->ptr[next]->keynum>s){    //�������ֵ��и���
         pr = ppar->ptr[next];
         p->key[++p->keynum] = ppar->key[next];       //pkey+1,
         if( NULL!=pr->ptr[0]){
              p->ptr[p->keynum] =  pr->ptr[0];        //�������²�ڵ㣬��pr��һ���Һ��ӱ��p�����һ������
              pr->ptr[0]->parent = p;                 //�ı��Һ��ӵ�˫��ֵ
         }
                
         ppar->key[next] = pr->key[1];
         pr->ptr[0] = pr->ptr[1];
         for(j = 1;j<pr->keynum;j++){                  //��pr���йؼ�������
              pr->key[j] = pr->key[j+1];         
              pr->ptr[j] = pr->ptr[j+1];         
         }
         pr->keynum--; 
     }else if(i>0&&ppar->ptr[last]->keynum>s){       //�������ֵ��и���
         pl = ppar->ptr[last]; 
         p->keynum++;
         for(j = p->keynum;j>1;j--){
            p->key[j] = p->key[j-1];
            p->ptr[j] = p->ptr[j-1];
         }
         p->key[1] = ppar->key[i];
         if(NULL!=pl->ptr[pl->keynum]){
            p->ptr[0] = pl->ptr[pl->keynum];        //�������²�ڵ㣬��pl����ӱ��p�ĵ�һ������
            pl->ptr[pl->keynum]->parent = p;        //����˫��
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
	printf("======�˵���=====\n");
	printf("1:������Ԫ��\n");
	printf("2:�������\n");
	printf("3:�������\n"); 
    printf("4:ɾ��Ԫ��\n");
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
        printf("���������: ");
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
                 printf("B��������\n");
                 break;
         default : 
               getchar();
               return 0;
        }     
     }    
      return 0;
     
}
