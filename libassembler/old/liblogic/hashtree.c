/*
 * ͷ�ļ�
 */
#include "hashtree.h"

/*
 * ��������
 */
/* �����ϣֵ */
dword_t hash(byte_t *, dword_t, dword_t);
/* �ݹ�Ѱ�ҹ�ϣ�ڵ� */
PHASHTREE_NODE find_node_hashtree(PHASHTREE_NODE, byte_t *,dword_t);
/* �����ϣ�ڵ� */
PHASHTREE_NODE insert_node_hashtree(PHASHTREE_NODE, PHASHTREE_NODE *, byte_t *, dword_t, void *, FPHashNodeValueFreeCallBack);
/* �ݹ�ɾ�����нڵ� */
void del_all_hashnodes(PHASHTREE_NODE *);
/* �����ϣ�ڵ����� */
dword_t count_sub_node_hashtree(PHASHTREE_NODE);
/* ��ϣ�ڵ�ת��Ϊ���� */
PHASHLIST_NODE hashnode_2_linkedlist(PHASHTREE_NODE node);

/*
 * ��������
 */
/*
 * ����:����
 * ��ʼ����:2008-12-29
 * �������:2008-12-29
 * ����:
 *    n:��ϣ�����ڵ�����.
 *    free_value:���ͷŽڵ�ʱ���õĻص�����.
 * ����ֵ:
 *	  ���ع�ϣ��ָ��.
 * ˵��:
 *    ��ʼ����ϣ��.
 */
PHASHTREE init_hashtree(dword_t n, FPHashNodeValueFreeCallBack free_value) {  
   	dword_t i;
	PHASHTREE_NODE root;
	PHASHTREE tree;
	if (!n) return NULL;
	tree = (PHASHTREE)logic_malloc(sizeof(HASHTREE));
	root = (PHASHTREE_NODE)logic_malloc(n * sizeof(HASHTREE_NODE));
   	for (i = 0; i < n; i++) {
      	root[i].empty = T;
		root[i].name = NULL;
		root[i].name_len = 0;
		root[i].parents = NULL;
      	root[i].left = NULL;
      	root[i].right = NULL;
   	}/* end for */
	tree->node_set = root;
	tree->node_count = 0;
	tree->prime = n;
	tree->free_value = free_value;
   	return tree;
}/* end init_hashtree */

/*
 * ����:����
 * ��ʼ����:2008-12-29
 * �������:2008-12-29
 * ����:
 *    tree:��ϣ��ָ���ָ��.
 * ����ֵ:
 *	  ��.
 * ˵��:
 *    �ͷŹ�ϣ��.
 */
void free_hashtree(PHASHTREE *ptree) {
	dword_t i;
	PHASHTREE tree = *ptree;
	PHASHTREE_NODE *pr = &(tree->node_set);
	assert(ptree && tree);
	if (!(*pr)) return;
	for (i = 0; i < tree->prime; i++) {
		if (((*pr)+i)->name) logic_free(((*pr)+i)->name);
		del_all_hashnodes(&(((*pr)+i)->left));
        del_all_hashnodes(&(((*pr)+i)->right));
		if (!(((*pr)+i)->empty)) {
			_free_value_hashnode_((*pr)+i);/* �ͷ�ֵ */
		}
	}/* end for */
	logic_free(*pr);
	*pr = NULL;
	logic_free(tree);
	*ptree = NULL;
	return;
}/* end free_hashtree */

/*
 * ����:����
 * ��ʼ����:2008-12-29
 * �������:2008-12-29
 * ����:
 *	  tree:��ϣ��ָ��.
 *    s:�ڵ���.
 *    n:�ڵ�������.
 *    v:Ҫ���ӵ�ֵָ��.
 * ����ֵ:
 *	  ��ӽڵ�ṹָ��.
 *    NULL:ʧ��.
 * ˵��:
 *    ���ӽڵ㵽��ϣ��.
 */
PHASHTREE_NODE add_node_hashtree(PHASHTREE tree, byte_t *s, dword_t n, void * v) {
	dword_t hash_value;
	PHASHTREE_NODE ptr;
	PHASHTREE_NODE root = tree->node_set;
	assert(tree && root && s && n);
   	hash_value = hash(s, n, tree->prime);
	/* ���� */
   	if (root[hash_value].empty) {
		root[hash_value].empty = F;
		if (root[hash_value].name) logic_free(root[hash_value].name);
		root[hash_value].name = (byte_t *)logic_malloc(n);
		root[hash_value].name_len = n;
      	memcpy(root[hash_value].name, s, n);
		root[hash_value].name_len = n;
		root[hash_value].value = v;
		root[hash_value].free_value = tree->free_value;
		root[hash_value].parents = NULL;
      	root[hash_value].left = NULL;
     	root[hash_value].right = NULL;
		(tree->node_count)++;
		return &root[hash_value];
	}
   	ptr = &root[hash_value];
   	ptr = insert_node_hashtree(ptr, &ptr, s, n, v, tree->free_value);
	if (ptr) (tree->node_count)++;
	return ptr;
}/* end add_node_hashtree */

/*
 * ����:����
 * ��ʼ����:2008-12-29
 * �������:2008-12-29
 * ����:
 *    parents:�ڵ�ĸ��ڵ�.
 *    link:Ҫ����Ľڵ�ָ��.
 *    s:�ڵ���.
 *    n:�ڵ�������.
 *    v:Ҫ���ӵ�ֵָ��.
 *    free_value:�ͷ�ֵ�ڴ�ص�����.
 * ����ֵ:
 *	  ����ڵ��ָ��.
 *    NULL:ʧ��.
 * ˵��:
 *    ����ڵ㵽��ϣ��.
 */
PHASHTREE_NODE insert_node_hashtree(PHASHTREE_NODE parents, PHASHTREE_NODE *link, byte_t *s, dword_t n, void * v, FPHashNodeValueFreeCallBack free_value) {
	assert(parents && link && s && n);
   	if (!(*link)) {	
		(*link) = (PHASHTREE_NODE)logic_malloc(sizeof(HASHTREE_NODE));
		(*link)->empty = F;
		if ((*link)->name) logic_free((*link)->name);
		(*link)->name = (byte_t *)logic_malloc(n);
		memcpy((*link)->name, s, n);
		(*link)->name_len = n;
		(*link)->value = v;
		(*link)->free_value = free_value;
		(*link)->parents = parents;
      	(*link)->left = NULL;
      	(*link)->right = NULL;
		return (*link);
   	} else if (memcmp((*link)->name, s, n) == 0) {   
		/* error codes
       	   the same name of variable */
		return NULL;
   	} else if(memcmp((*link)->name, s, n) > 0)
      	return insert_node_hashtree(*link, &((*link)->left), s, n, v, free_value);
   	else
		return insert_node_hashtree(*link, &((*link)->right), s, n, v, free_value);
}/* end inserinsert_node_hashtreet_hashnode */

/*
 * ����:����
 * ��ʼ����:2008-12-29
 * �������:2008-12-29
 * ����:
 *    s:�ڵ���.
 *    n:�ڵ�������.
 *    prime:��Χ.
 * ����ֵ:
 *	 ��ϣֵ.
 * ˵��:
 *    �����ϣֵ.
 */
dword_t hash(byte_t *s, dword_t n, dword_t prime) {
	byte_t *p;
	byte_t h = 0, g;
	dword_t i;
	for (p = s, i = 0; i<n; p = p + 1, i++) {
		h = (h << 4) + (*p);
		if (g = (h & 0xf0000000)) {
			h = h ^ (g >> 24);
			h = h ^ g;
		}
	}
	return h % prime;
}/* end hash */

/*
 * ����:����
 * ��ʼ����:2008-12-29
 * �������:2008-12-29
 * ����:
 *    tree:��ϣ��ָ��.
 *    s:�ڵ���.
 *    n:�ڵ�������
 * ����ֵ:
 *	 ��ϣ�ڵ�ָ��.
 * ˵��:
 *    ��ѯ��ϣ�ڵ�.
 */
PHASHTREE_NODE query_node_hashtree(PHASHTREE tree, byte_t *s, dword_t n) {
	dword_t hash_value;
	PHASHTREE_NODE ptr;
	PHASHTREE_NODE root = tree->node_set;
	assert(tree && root && s && n);
   	hash_value = hash(s, n, tree->prime);
   	if (root[hash_value].empty) return NULL;
	ptr = find_node_hashtree(&root[hash_value], s, n);
   	return ptr;
}/* end query_node_hashtree */

/*
 * ����:����
 * ��ʼ����:2008-12-29
 * �������:2008-12-29
 * ����:
 *    link:�ڵ�ָ��.
 *    s:�ڵ���.
 *    n:�ڵ�������.
 * ����ֵ:
 *	 ��ϣ�ڵ�ָ��.
 * ˵��:
 *    �ݹ�Ѱ�ҹ�ϣ�ڵ�.
 */
PHASHTREE_NODE find_node_hashtree(PHASHTREE_NODE link, byte_t *s, dword_t n) {
	PHASHTREE_NODE ptr = NULL;
	assert(link && s && n);
   	if (!link) ;
   	else if(memcmp(link->name, s, n) == 0)
		ptr = link;
   	else if(memcmp(link->name, s, n) > 0)
		ptr = find_node_hashtree(link->left, s, n);
   	else
		ptr = find_node_hashtree(link->right, s, n);
	return ptr;
}/* end find_hashnode */

/*
 * ����:����
 * ��ʼ����:2008-12-29
 * �������:2008-12-29
 * ����:
 *    link:ָ��ڵ�ָ���ָ��.
 * ����ֵ:
 *    ��.
 * ˵��:
 *    �ݹ�ɾ�����й�ϣ�ڵ�.
 */
void del_all_hashnodes(PHASHTREE_NODE *link) {
	if (*link) {
		if ((*link)->name) logic_free((*link)->name);
		del_all_hashnodes(&((*link)->left));
		del_all_hashnodes(&((*link)->right));
		_free_value_hashnode_(*link);
		logic_free(*link);
		*link = NULL;
	}/* end if */
	return;
}/* end del_all_hashnodes */

/*
 * ����:����
 * ��ʼ����:2009-1-3
 * �������:2009-1-8
 * ����:
 *    tree:��ϣ��ָ��.
 *    s:�ڵ������.
 *    n:�ڵ����ĳ���.
 * ����ֵ:
 *    T:ɾ���ɹ�.
 *    F:ɾ��ʧ��.
 * ˵��:
 *	  ɾ���ڵ�˵��,�����ɾ���Ľڵ��������������ӽڵ�
 *    �������Һ��ӽڵ�ĸ��ڵ�����Ϊ������ĸ��ڵ�
 *    �����������ӽڵ����ӵ����Һ��ӽڵ�������.
 *    �����û���Һ��ӽڵ���ֱ�ӽ��������ӽڵ�ĸ�
 *    �ڵ����ӵ����ĸ��ڵ�.
 *    �����û�����ӽڵ���ֱ�ӽ������Һ��ӽڵ�ĸ�
 *    �ڵ����ӵ����ĸ��ڵ�.
 *    ͬʱ���������ڵ������.
 *    ���ɾ���Ľڵ�û�и��ڵ���������,ֱ�ӽ�������
 *    �ڵ�ĸ��ڵ�����Ϊ��.
 *
 * ע��:
 *    ����ɾ���㷨���������ڵ���䲻����.
 */
/* ��ڵ�Ϊ��,�ҽڵ㲻Ϊ�� */
#define L0R1_DEL_NODE_HASHTREE		0x00
/* ��ڵ㲻Ϊ��,�ҽڵ�Ϊ�� */
#define L1R0_DEL_NODE_HASHTREE		0x01
/* ��ڵ㲻Ϊ��,�ҽڵ㲻Ϊ�� */
#define L1R1_DEL_NODE_HASHTREE		0x02
/* ��ڵ�Ϊ��,�ҽڵ�Ϊ�� */
#define L0R0_DEL_NODE_HASHTREE		0x03
bool_t del_node_hashtree(PHASHTREE tree, byte_t *s, dword_t n) {
	PHASHTREE_NODE ptr, parents;
	byte_t a;
	assert(tree && s && n);
	if (!(ptr = query_node_hashtree(tree, s, n))) return F;
	parents = ptr->parents;
	/* �鿴��ǰҪɾ���ڵ����ӷ�ʽ */
	if ((!(ptr->left)) && (ptr->right)) a = L0R1_DEL_NODE_HASHTREE;
	else if ((ptr->left) && (!(ptr->right))) a = L1R0_DEL_NODE_HASHTREE;
	else if ((ptr->left) && (ptr->right)) a = L1R1_DEL_NODE_HASHTREE;
	else if ((!(ptr->left)) && (!(ptr->right))) a = L0R0_DEL_NODE_HASHTREE;
	switch (a) {
		case L0R1_DEL_NODE_HASHTREE:{
			PHASHTREE_NODE exchange = ptr->right;
			if (!parents) {
				if (ptr->name) logic_free(ptr->name);
				_copy_node_hashtree_(ptr, exchange);
				ptr->parents = NULL;
				_free_value_hashnode_(exchange);
				logic_free(exchange);
			} else {
				if (parents->left == ptr) parents->left = exchange;
				else parents->right = exchange;
				exchange->parents = parents;
				if (ptr->name) logic_free(ptr->name);
				_free_value_hashnode_(ptr);
				logic_free(ptr);
			}/* end if */
			(tree->node_count)--;
			return T;
		}break;
		case L1R0_DEL_NODE_HASHTREE:{
			PHASHTREE_NODE exchange = ptr->left;
			if (!parents) {
				if (ptr->name) logic_free(ptr->name);
				_copy_node_hashtree_(ptr, exchange);
				ptr->parents = NULL;
				_free_value_hashnode_(exchange);
				logic_free(exchange);
			} else {
				if (parents->left == ptr) parents->left = exchange;
				else parents->right = exchange;
				exchange->parents = parents;
				if (ptr->name) logic_free(ptr->name);
				_free_value_hashnode_(ptr);
				logic_free(ptr);
			}/* end if */
			(tree->node_count)--;
			return T;		
		}break;
		case L1R1_DEL_NODE_HASHTREE:{
			PHASHTREE_NODE exchange_left = ptr->left;
			PHASHTREE_NODE exchange_right = ptr->right;
			PHASHTREE_NODE exchange = exchange_right;
			while (exchange->left) exchange = exchange->left;
			exchange->left = exchange_left;
			exchange_left->parents = exchange->left;
			if (!parents) {
				if (ptr->name) logic_free(ptr->name);
				_copy_node_hashtree_(ptr, exchange_right);
				ptr->parents = NULL;
				_free_value_hashnode_(exchange_right);
				logic_free(exchange_right);
			} else {
				if (parents->left == ptr) parents->left = exchange_right;
				else parents->right = exchange_right;
				exchange_right->parents = parents;
				if (ptr->name) logic_free(ptr->name);
				_free_value_hashnode_(ptr);
				logic_free(ptr);
			}/* end if */
			(tree->node_count)--;
			return T;	
		}break;
		case L0R0_DEL_NODE_HASHTREE:{
			if (!parents) {
				if (ptr->name) logic_free(ptr->name);
				_free_value_hashnode_(ptr);
				_clean_node_hashtree_(ptr);
				ptr->empty = T;
			} else {
				if (parents->left == ptr) parents->left = NULL;
				else parents->right = NULL;
				if (ptr->name) logic_free(ptr->name);
				_free_value_hashnode_(ptr);
				logic_free(ptr);
			}/* end else */
			(tree->node_count)--;
			return T;
		}break;
	}/* end switch */
	return F;
}/* end del_node_hashtree */

/*
 * ����:����
 * ��ʼ����:2009-1-7
 * �������:2009-1-7
 * ����:
 *    f:�ڵ���ָ��.
 * ����ֵ:
 *    �ڵ�����.
 * ˵��:
 *	  �����ϣ�ӽڵ���Ŀ.
 */
dword_t count_sub_node_hashtree(PHASHTREE_NODE f) {
	dword_t count = 0;
	if (f->left) count += count_sub_node_hashtree(f->left);
	if (f->right) count += count_sub_node_hashtree(f->right);
	count++;
#ifdef DEBUG_HASHTREE
	dword_t i;
	printf("hashtree_node->name = ");
	for (i = 0; i < f->name_len; i++)
		printf("0x%2X", f->name[i]);
	printf("\n");
	printf("hashtree_node->left = 0x%X\n", f->left);
	printf("hashtree_node->right = 0x%X\n", f->right);
	printf("--------------------\n");
#endif
	return count;
}/* end count_sub_node_hashtree */
/*
 * ����:����
 * ��ʼ����:2009-1-7
 * �������:2009-1-7
 * ����:
 *    tree:��ϣ��ָ��.
 * ����ֵ:
 *    �ڵ�����.
 * ˵��:
 *	  �����ϣ�ڵ���Ŀ.
 */
dword_t count_node_hashtree(PHASHTREE tree) {
	dword_t i, count = 0;
	PHASHTREE_NODE root = tree->node_set;
	assert(tree && root);
	for (i = 0; i < tree->prime; i++) {
		if (!root[i].empty) {
			if (root[i].left) count += count_sub_node_hashtree(root[i].left);
			if (root[i].right) count += count_sub_node_hashtree(root[i].right);
			count++;
#ifdef DEBUG_HASHTREE
			dword_t j;
			printf("hashtree_node->name = ");
			for (j = 0; j < root[i].name_len; j++)
				printf("0x%2X", root[i].name[j]);
			printf("\n");
			printf("hashtree_node->left = 0x%X\n", root[i].left);
			printf("hashtree_node->right = 0x%X\n", root[i].right);
			printf("--------------------\n");
#endif
		}/* end if */
	}/* end for */
	return count;
}/* end count_node_hashtree */

/*
 * ����:����
 * ��ʼ����:2009-6-13
 * �������:2009-6-13
 * ����:
 *    node:Ҫ��ȡ�Ĺ�ϣ�ڵ���ָ��.
 * ����ֵ:
 *    ���ػ�ȡ������.
 *    ʧ��:NULL.
 * ˵��:
 *	  ��ϣ�ڵ�ת��Ϊ����.
 */
PHASHLIST_NODE hashnode_2_linkedlist(PHASHTREE_NODE node) {
	PHASHLIST_NODE p = NULL;
	PHASHLIST_NODE *pp = &p;
	if (!node) return NULL;
	(*pp) = (PHASHLIST_NODE)logic_malloc(sizeof(HASHLIST_NODE));
	(*pp)->next = NULL;
	(*pp)->hashnode = node;
	/* ֻ��¼���µ�ָ��,���ϵ�ָ�뷵�غ��и�������ֵ */
	if (node->left) (*pp)->next = hashnode_2_linkedlist(node->left);
	/* �ƶ����ڵ�ĩβ */
	while ((*pp)->next) pp = &((*pp)->next);
	if (node->right) (*pp)->next = hashnode_2_linkedlist(node->right);
	return p;
}/* end hashnode_2_linkedlist */
/*
 * ����:����
 * ��ʼ����:2009-6-13
 * �������:2009-6-13
 * ����:
 *    tree:��ϣ��ָ��.
 * ����ֵ:
 *    ת��������
 *    ת��ʧ�ܻ����޽ڵ���ΪNULL.
 * ˵��:
 *	  ����ϣ��ת��Ϊ����.
 *
 */
PHASHLIST_NODE hashtree_2_linkedlist(PHASHTREE tree) {
	dword_t i;
	PHASHTREE_NODE nodes = tree->node_set;
	PHASHLIST_NODE p = NULL;
	PHASHLIST_NODE *pp = &p;
	if (!(tree->node_count)) return NULL;
	assert(tree && nodes);
	for (i = 0; i < tree->prime; i++) {
		if (nodes[i].empty) continue;
		/* ���Ȼ�ȡ��ǰ�ڵ� */
		(*pp) = (PHASHLIST_NODE)logic_malloc(sizeof(HASHLIST_NODE));
		(*pp)->hashnode = &nodes[i];
		(*pp)->next = NULL;
		if (nodes[i].left) (*pp)->next = hashnode_2_linkedlist(nodes[i].left);
		/* �ƶ���ĩβ */
		while ((*pp)->next) pp = &((*pp)->next);
		if (nodes[i].right) (*pp)->next = hashnode_2_linkedlist(nodes[i].right);
		/* �ٴ��ƶ���ĩβ */
		while ((*pp)->next) pp = &((*pp)->next);
	}/* end for */
	return p;
}/* end hashtree_2_linkedlist */

/*
 * ����:����
 * ��ʼ����:2009-6-13
 * �������:2009-6-13
 * ����:
 *    pn:��ϣ�������ָ��.
 * ����ֵ:
 * ˵��:
 *	  �ͷ��ɹ�ϵ��ת���ɵ�����.
 */
void free_hashtree_linkedlist(PHASHLIST_NODE *pn) {
	PHASHLIST_NODE cur = *pn;
	PHASHLIST_NODE t = NULL;
	assert(pn && cur);
	while (cur != NULL) {
		t = cur->next;
		logic_free(cur);
		cur = t;
	}
	*pn = NULL;
}/* end free_hashtree_linkedlist */