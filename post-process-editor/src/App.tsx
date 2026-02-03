import 'reactflow/dist/style.css';
import './App.css';
import { useStore } from './store';
import PassNode from './nodes/PassNode';
import InputNode from './nodes/InputNode';
import OutputNode from './nodes/OutputNode';
import TextureNode from './nodes/TextureNode';
import ReactFlow, { Background, Controls, Panel } from 'reactflow';
import type { Connection, Edge } from 'reactflow';

import type { PassNodeData } from './types';

// 1. 移到组件外部，作为一个纯工具函数
const checkCycle = (connection: Connection, edges: Edge[]) => {
  const target = connection.target;
  const source = connection.source;
  const stack = [target];
  const visited = new Set();

  while (stack.length > 0) {
    const node = stack.pop();
    if (node === source) return true;
    if (!visited.has(node)) {
      visited.add(node);
      edges
        .filter((edge) => edge.source === node)
        .forEach((edge) => stack.push(edge.target));
    }
  }
  return false;
};


const nodeTypes = {
  passNode: PassNode,
  inputNode: InputNode,
  outputNode: OutputNode,
  textureNode: TextureNode, // 注册贴图节点
};

export default function App() {
  const {
    nodes, edges, onNodesChange, onEdgesChange, onConnect,
    addPassNode, addTextureNode, generateJSON,
    postProcessName, setPostProcessName
  } = useStore();

  const isValidConnection = (connection: Connection) => {
    if (connection.source === connection.target) return false;

    // 1. 环路检查
    if (checkCycle(connection, edges)) return false;

    const sourceNode = nodes.find((n) => n.id === connection.source);
    const targetNode = nodes.find((n) => n.id === connection.target);

    if (!sourceNode || !targetNode) return false;

    // 2. 材质连线校验 (TextureNode -> PassNode 的 Texture 端口)
    if (sourceNode.type === 'textureNode') {
      if (targetNode.type !== 'passNode') return false;
      const data = targetNode.data as PassNodeData;

      // 检查 targetHandle 是否匹配某个 texturePort 的 id
      return data.texturePorts?.some(p => p.id === connection.targetHandle);
    }

    // 3. RT 连线校验 (InputNode/PassNode -> PassNode 的 Input 端口 或 OutputNode)
    if (sourceNode.type === 'inputNode' || sourceNode.type === 'passNode') {
      // 如果连向输出节点，直接允许
      if (targetNode.type === 'outputNode') return true;

      // 如果连向另一个 Pass 节点
      if (targetNode.type === 'passNode') {
        const data = targetNode.data as PassNodeData;

        // 检查 targetHandle 是否匹配某个 inputPort 的 id
        return data.inputPorts?.some(p => p.id === connection.targetHandle);
      }
    }

    return false;
  };

  const onExport = () => {
    const json = generateJSON();
    const blob = new Blob([JSON.stringify(json, null, 4)], { type: 'application/json' });
    const url = URL.createObjectURL(blob);
    const link = document.createElement('a');
    link.href = url;
    link.download = `${postProcessName}.json`;
    link.click();
  };

  return (
    <div style={{ width: '100vw', height: '100vh', background: '#121212' }}>
      <ReactFlow
        nodes={nodes}
        edges={edges}
        onNodesChange={onNodesChange}
        onEdgesChange={onEdgesChange}
        onConnect={onConnect}
        nodeTypes={nodeTypes}
        isValidConnection={isValidConnection}
        fitView
      >
        <Background color="#333" gap={20} />
        <Controls />

        {/* 顶部左侧：修改全局名字 */}
        <Panel position="top-left" style={{ display: 'flex', alignItems: 'center', gap: '15px' }}>
          <h2 style={{ color: '#fff', margin: 0, fontSize: '18px' }}>后处理编辑器</h2>
          <div style={{ display: 'flex', flexDirection: 'column' }}>
            <span style={{ color: '#666', fontSize: '9px' }}>PostProcess Name:</span>
            <input
              value={postProcessName}
              onChange={(e) => setPostProcessName(e.target.value)}
              style={{
                background: '#1a1a1a', border: '1px solid #444', color: '#3fb950',
                padding: '4px 8px', borderRadius: '4px', outline: 'none', fontWeight: 'bold'
              }}
            />
          </div>
        </Panel>

        {/* 顶部右侧：操作按钮 */}
        <Panel position="top-right" style={{ display: 'flex', gap: '10px' }}>
          <button onClick={addPassNode} style={btnStyle}>+ 添加 Pass 节点</button>

          {/* 新增：添加贴图节点的按钮 */}
          <button onClick={addTextureNode} style={{ ...btnStyle, borderColor: '#3a8ee6', color: '#3a8ee6' }}>
            + 添加 贴图节点
          </button>

          <button onClick={onExport} style={{ ...btnStyle, background: '#3fb950', color: '#fff', border: 'none' }}>
            导出 JSON
          </button>
        </Panel>
      </ReactFlow>
    </div>
  );
}

const btnStyle = {
  padding: '8px 16px',
  background: '#222',
  color: '#ccc',
  border: '1px solid #444',
  borderRadius: '4px',
  cursor: 'pointer',
  fontSize: '12px',
  fontWeight: 'bold',
  transition: 'all 0.2s'
};