import React, { useState } from 'react';
import { Handle, Position } from 'reactflow';
import type { NodeProps } from 'reactflow';
import { useStore } from '../store';
import type { TextureNodeData } from '../types';
const isLightColor = (hex: string) => {
    const r = parseInt(hex.slice(1, 3), 16);
    const g = parseInt(hex.slice(3, 5), 16);
    const b = parseInt(hex.slice(5, 7), 16);
    const brightness = (r * 299 + g * 587 + b * 114) / 1000;
    return brightness > 155;
};
export default function TextureNode({ id, data }: NodeProps<TextureNodeData>) {
    const { updateNodeData, setNodeThemeColor } = useStore();
    const [preview, setPreview] = useState<string | null>(null);
    const theme = data.themeColor || '#3a8ee6';
    const isThemeLight = isLightColor(theme);
    const textColor = isThemeLight ? '#000' : '#fff';

    // 处理拖拽移入
    const onDragOver = (e: React.DragEvent) => {
        e.preventDefault();
        e.stopPropagation();
    };

    // 处理文件放下
    const onDrop = (e: React.DragEvent) => {
        e.preventDefault();
        e.stopPropagation();

        const file = e.dataTransfer.files[0];
        if (file && file.type.startsWith('image/')) {
            // 1. 创建本地预览
            const reader = new FileReader();
            reader.onload = (event) => {
                setPreview(event.target?.result as string);
            };
            reader.readAsDataURL(file);

            // 2. 更新 Store 里的路径 (假设资源存放在 assets/textures/)
            const simulatedPath = `assets/textures/${file.name}`;
            updateNodeData(id, { path: simulatedPath, output: simulatedPath });
        }
    };

    return (
        <div
            onDragOver={onDragOver}
            onDrop={onDrop}
            className="texture-node"
            style={{
                background: '#2a2a2a',
                color: '#fff',
                width: '150px',
                borderRadius: '4px',
                border: `1px solid ${theme}`,
                overflow: 'hidden',
                boxShadow: `0 4px 15px ${theme}33`,
                display: 'flex',
                flexDirection: 'column'
            }}
        >
            {/* 标题栏 */}
            <div style={{
                background: theme,
                padding: '4px 8px',
                display: 'flex',
                justifyContent: 'space-between',
                alignItems: 'center'
            }}>
                <span style={{ fontSize: '10px', fontWeight: 'bold', color: textColor }}>TEXTURE</span>
                <div style={{ position: 'relative', width: '14px', height: '14px' }}>
                    <div style={{
                        width: '100%', height: '100%', borderRadius: '00%', // 改为圆形区分
                        background: theme, color: textColor, border: '1px solid', pointerEvents: 'none'
                    }} />
                    <input type="color" className="nodrag" value={theme}
                        onChange={(e) => setNodeThemeColor(id, e.target.value)}
                        style={{ position: 'absolute', top: 0, left: 0, width: '100%', height: '100%', opacity: 0, cursor: 'pointer' }}
                    />
                </div>
            </div>

            {/* 图片预览区域 */}
            <div style={{
                height: '120px',
                background: '#1a1a1a',
                display: 'flex',
                alignItems: 'center',
                justifyContent: 'center',
                position: 'relative',
                borderBottom: '1px solid #333'
            }}>
                {preview ? (
                    <img src={preview} alt="preview" style={{ width: '100%', height: '100%', objectFit: 'cover' }} />
                ) : (
                    <div style={{ textAlign: 'center', color: '#555', fontSize: '9px', padding: '10px' }}>
                        拖入图片文件 <br /> 或修改下方路径
                    </div>
                )}
            </div>

            {/* 路径信息 */}
            <div style={{ padding: '8px' }}>
                <input
                    className="nodrag"
                    value={data.path}
                    onChange={(e) => updateNodeData(id, { path: e.target.value, output: e.target.value })}
                    style={{
                        background: '#111',
                        border: '1px solid #333',
                        color: '#3a8ee6',
                        fontSize: '9px',
                        width: '100%',
                        padding: '2px 4px',
                        borderRadius: '2px',
                        outline: 'none',
                        boxSizing: 'border-box'
                    }}
                />
            </div>

            {/* 输出圆点 (蓝色) */}
            <Handle
                type="source"
                position={Position.Right}
                id="output"
                style={{ background: theme, width: '10px', height: '10px', right: '-6px', border: '2px solid #2a2a2a' }}
            />
        </div>
    );
}