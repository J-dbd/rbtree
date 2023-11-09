#include "rbtree.h"
#include <stdlib.h>
#include <stdio.h>


///////////////////////////////
// CREATE & INSERT OPERATION //
//     생성 및 삽입 연산      //
///////////////////////////////

//Create_생성
rbtree *new_rbtree(void)
{
  //트리에 동적으로 메모리 할당
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  //닐 노드에 메모리 할당 및 닐 노드 세팅
  node_t *temp = (node_t *)calloc(1, sizeof(node_t));
  temp->color = RBTREE_BLACK;
  p->nil = temp;
  // 루트 노드 세팅
  p->root = p->nil;
  p->root->right = temp;
  p->root->left = temp;
  p->root->parent = temp;
  //부모/양쪽 자식에 닐 노드를 세팅한다.

  return p;
}

//삽입 연산
node_t *rbtree_insert(rbtree *t, const key_t key)
{
  // null check
  if (t == NULL)
  {
    //이 함수에 대한 입력값이 올바르지 않으므로 null을 리턴
    return NULL;
  }

  // set pointer x and y
  //여기서 포인터는 주소를 가리키는 포인터가 아님
  node_t *x = t->root;
  node_t *y = t->nil;

  // 함수가 종료되어도 남아있어야 하므로 메모리를 동적으로 할당해준다.
  node_t* z = (node_t*)malloc(sizeof(node_t));
  z->key = key;

 //root가 nil이 아닐 때
  while (x != t->nil)
  {
    y = x; //포인터 위치 교환
    // 입력된 값이 루트값보다 작으면
    // 왼쪽 크면 오른쪽에 삽입
    if (key < x->key)
    {
      x = x->left;
    }
    else
    {
      x = x->right;
    }
  }

  // root가 nil을 가리킬때: 
  // case 1. 아직 아무것도 없을 때 -> z의 부모에 nil을 넣어줌.
  // case 2. y가 트리의 leaf 까지 온 것이므로 y에 z를 달아준다. (y를 부모 삼음으로써)
  z->parent = y;
  
  // case 1일 경우
  if (y == t->nil)
  {
    //z를 루트에 넣어 줌
    t->root = z;
  }

  //case 2일 경우 key값에 따라 BST의 원리로 왼/오 자식에 넣어준다.
  else if (z->key < y->key)
  {
    y->left = z;
  }
  else
  {
    y->right = z;
  }
  // set new node's key and color
  z->left = t->nil;
  z->right = t->nil;
  z->color = RBTREE_RED;
  
  //fixup으로 삽입한 노드의 색울 조정
  rbtree_insert_fixup(t, z);

  return t->root;
}

// 삽입 수정
void rbtree_insert_fixup(rbtree *t, node_t *z)
{
  // y 라는 노드를 임시로 세팅
  node_t *y = NULL;

  //주어진 z라는 노드의 부모의 색이 RED일 때 계속해서 반복 시작
  // RED confilict!
  while (z->parent->color == RBTREE_RED)
  {
    //parent가 grandparent의 왼쪽 자식일 때
    if (z->parent == z->parent->parent->left)
    {
      //y(포인터)를 right uncle로 세팅
      y = z->parent->parent->right;
      // 만약 right uncle이 red라면? (+현재 부모의 색은 red)
      // => re-coloring!
      if (y->color == RBTREE_RED)
      // uncle과 parent가 red라면 
      // uncle&parent -> black
      // grandparent -> red
      {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }
      // 만약 right uncle이 black이라면? (+현재 부모의 색은 red)
      // -> rotate!
      else
      {
        // 좌회전
        if (z == z->parent->right)
        {
          z = z->parent;
          left_rotate(t, z);
        }
        //우회전
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        right_rotate(t, z->parent->parent);
      }
    }
    //parent가 grandparent의 오른쪽 자식일 때
    else
    {
      //y(포인터)를 left uncle로 세팅
      // 위와 동일, 좌우만 바뀌었다.
      y = z->parent->parent->left;

      if (y->color == RBTREE_RED)
      {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }

      else
      {
        if (z == z->parent->left)
        {
          z = z->parent;
          right_rotate(t, z);
        }
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        left_rotate(t, z->parent->parent);
      }
    }
  }
  //트리의 루트 노드 색은 검은 색이어야 한다.
  t->root->color = RBTREE_BLACK;
};

//////////////////////
// ROTATE OPERATION //
//////////////////////

// 좌회전
void left_rotate(rbtree *t, node_t *x)
{
  //node_t x 는 회전의 중심이자
  //회전에 연관된 3개의 노드 중 grandparent.

  //입력받은 노드의 right child를 노드 y에 세팅
  node_t *y = x->right;
  //right child의 left child를 붙인다. 
  x->right = y->left;

  
  if (y->left != t->nil)
  {
    y->left->parent = x;
  }
  y->parent = x->parent;

  if (x->parent == t->nil)
  {
    t->root = y;
  }
  else if (x == x->parent->left)
  {
    x->parent->left = y;
  }

  else
  {
    x->parent->right = y;
  }

  y->left = x;
  x->parent = y;
};
// 우회전
void right_rotate(rbtree *t, node_t *x)
{

  node_t *y = x->left;

  x->left = y->right;

  if (y->right != t->nil)
  {
    y->right->parent = x;
  }
  y->parent = x->parent;

  if (x->parent == t->nil)
  {
    t->root = y;
  }
  else if (x == x->parent->right)
  {
    x->parent->right = y;
  }

  else
  {
    x->parent->left = y;
  }
  y->right = x;
  x->parent = y;
};

//////////////////////
// DELETE OPERATION //
//    삭제 연산      //
//////////////////////

//이식하는 함수
void rb_transplant(rbtree *t, node_t *u, node_t *v)
{
  if (u->parent == t->nil)
  {
    t->root = v;
  }
  else if (u == u->parent->left)
  {
    u->parent->left = v;
  }
  else
  {
    u->parent->right = v;
  }
  v->parent = u->parent;
}

// Finding minimum
node_t *tree_minimun(node_t *p, rbtree *t)
{
  while (p->left != NULL && p->left != t->nil)
  {
    p = p->left;
  }
  return p;
}

// fixup function
void rbtree_erase_fixup(rbtree *t, node_t *p)
{
  node_t *x = p;
  node_t *w = NULL;

  while (x != t->root && x->color == RBTREE_BLACK)
  {
    if (x == x->parent->left)
    {
      w = x->parent->right;

      if (w->color == RBTREE_RED)
      {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(t, x->parent);
        w = x->parent->right;
      }
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK)
      {
        w->color = RBTREE_RED;
        x = x->parent;
      }
      else 
      {if (w->right->color == RBTREE_BLACK)
      {
        w->left->color = RBTREE_BLACK;
        w->color = RBTREE_RED;
        right_rotate(t, w);
        w = x->parent->right;
      }
      w->color = x->parent->color;
      x->parent->color = RBTREE_BLACK;
      w->right->color = RBTREE_BLACK;
      left_rotate(t, x->parent);
      x = t->root;
     }
    }

    else
    {
      w = x->parent->left;
      if (w->color == RBTREE_RED)
      {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t, x->parent);
        w = x->parent->left;
      }
      if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK)
      {
        w->color = RBTREE_RED;
        x = x->parent;
      }
      else {
      if (w->left->color == RBTREE_BLACK)
      {
        w->right->color = RBTREE_BLACK;
        w->color = RBTREE_RED;
        left_rotate(t, w);
        w = x->parent->left;
      }
      w->color = x->parent->color;
      x->parent->color = RBTREE_BLACK;
      w->left->color = RBTREE_BLACK;
      right_rotate(t, x->parent);
      x = t->root;
     }
    }
  }
  x->color = RBTREE_BLACK;
};

// erase(delete) a target node
int rbtree_erase(rbtree *t, node_t *p)
{
  node_t *x;
  node_t *y = p;
  color_t original_color = y->color;

  node_t *z = p;

  if (z->left == t->nil)
  {
    x = z->right;
    rb_transplant(t, z, z->right);
  }
  else if (z->right == t->nil)
  {
    x = z->left;
    rb_transplant(t, z, z->left);
  }
  else
  {
    y = tree_minimun(y->right, t);
    original_color = y->color;
    x = y->right;

    if (y->parent == z)
    {
      x->parent = y;
    }

    else
    {
      rb_transplant(t, y, y->right);
      y->right = z->right;
      y->right->parent = y;
    }

    rb_transplant(t, z, y);
    y->left = z->left;
    y->left->parent = y;
    y->color = z->color;
  }

  if (original_color == RBTREE_BLACK)
  {
    rbtree_erase_fixup(t, x);
  }
  return 0;
}


// rm node by node 
void delete_nodes(node_t* node, node_t* NIL){
  if (node!=NIL){
    delete_nodes(node->left, NIL);
    delete_nodes(node->right, NIL);
   
   free(node);
  }

}

// remove all tree
void delete_rbtree(rbtree *t)
{
  delete_nodes(t->root, t->nil);
  free(t->nil);
  free(t);
}


//////////////////////
// SEARCH OPERATION //
//////////////////////

//find target node
node_t *rbtree_find(const rbtree *t, const key_t key)
{
  node_t *node = t->root;
  while (node->key != key && node != t->nil)
  {

    if (node->key < key)
    {
      node = node->right;
    }
    else if (node->key > key)
    {
      node = node->left;
    }
  }

  if (node != t->nil && node->key ==key){
    return node;
  }
  //return t->nil;
  return NULL;
}

//find the node with a min value(key)
node_t *rbtree_min(const rbtree *t)
{
  node_t *node = t->root;
  while (node->left != NULL && node->left != t->nil)
  {
    node = node->left;
  }
  return node;
}

//find the node with a max value(key)
node_t *rbtree_max(const rbtree *t)
{
  node_t *node = t->root;
  while (node->right != NULL && node->right != t->nil)
  {
    node = node->right;
  }
  return node;
}

/////////////////////
// ARRAY OPERATION //
/////////////////////

void inorderTraversal(node_t* root, key_t *arr, int* idx, node_t* NIL){
  if(root==NULL|| root==NIL){
    return;
  }

  inorderTraversal(root->left, arr, idx, NIL);
  arr[(*idx)++]=root->key; // ?
  inorderTraversal(root->right, arr, idx, NIL);

}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{
  node_t* root=t->root;
  int idx = 0;
  inorderTraversal(root, arr, &idx, t->nil);
  if(idx!=n){
    return -1;
  }
  return 0;
}
